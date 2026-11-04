#include "Enemies.hpp"
#include "EnemyData.hpp"

#include <algorithm>
#include <stdexcept>

namespace gyrus::game {
EnemyPaths::EnemyPaths() : descriptors(enemy_data::descriptors) {}

EnemyState::EnemyState() {
  // Captured original game-start RAM at $0552..$0575 and $05AA..$05CD.
  constexpr std::array<std::uint8_t, 12> angles{116,106,96,84,74,64,52,42,32,20,10,0};
  for (unsigned i = 0; i < formation.size(); ++i) {
    formation[i].angle = angles[i % 12];
    formation[i].depth = 52 - (i / 12) * 4;
    formation[i].spriteType = 22;
  }
}

namespace {
void hide(EnemyActor& actor) {
  actor.visible = false;
  actor.hit = false;
  actor.width = 0;
}

bool place(EnemyActor& actor) {
  if (actor.depth >= 64 || actor.spriteType >= 58) { hide(actor); return false; }
  const auto point = project(actor.angle, actor.depth);
  if (point.clipped || point.x >= 160 || point.y >= 96) { hide(actor); return false; }
  actor.position = point;
  const auto& profile = enemy_data::spriteProfiles[actor.spriteType / 2];
  const auto packed = profile[1 + (actor.depth >> 4)];
  actor.width = ((actor.depth & 8) ? packed : (packed >> 4)) & 15;
  if (profile[0] < 128) actor.heading = profile[0] ? actor.angle >> (profile[0] + 2) : 0;
  const unsigned left = point.x > actor.width / 2 ? point.x - actor.width / 2 : 0;
  const unsigned top = point.y > actor.width / 2 ? point.y - actor.width / 2 : 0;
  if (!actor.width || left + actor.width >= 160 || top + actor.width >= 96) {
    hide(actor);
    return false;
  }
  actor.visible = true;
  actor.renderedDepth = actor.depth;
  actor.renderedSpriteType = actor.spriteType;
  actor.renderedHeading = actor.heading;
  return true;
}

void append(EnemyEvents& target, const EnemyEvents& other) {
  target.scoreUnits += other.scoreUnits;
  target.scoreAwards.insert(target.scoreAwards.end(), other.scoreAwards.begin(), other.scoreAwards.end());
  target.sounds.insert(target.sounds.end(), other.sounds.begin(), other.sounds.end());
  target.bonuses.insert(target.bonuses.end(), other.bonuses.begin(), other.bonuses.end());
}
} // namespace

bool projectEnemyActor(EnemyActor& actor) { return place(actor); }

EnemyEvents updateIncomingLane(EnemyPaths& paths, EnemyState& enemies,
                               unsigned parity, const EnemyInputs& inputs) {
  if (parity > 1) throw std::out_of_range("Enemy lane parity must be zero or one");
  EnemyEvents events;
  if (parity) paths.oddTimer = paths.cadence;
  else paths.evenTimer = paths.cadence;
  for (int index = 8 + int(parity); index >= 0; index -= 2) {
    auto& actor = enemies.incoming[index];
    auto& phase = paths.phases[index];
    auto& cursor = paths.cursors[index];
    if (!phase) continue;
    auto remove = [&] {
      --paths.incomingCount;
      phase = 0;
      actor.formationSlotReserved = false;
      hide(actor);
    };
    auto arrive = [&] {
      paths.scoreChain &= 127;
      --paths.incomingCount;
      ++enemies.formationCount;
      phase = 0;
      hide(actor);
      if (!actor.formationSlot || actor.formationSlot > enemies.formation.size())
        throw std::out_of_range("Original formation slot outside observed range");
      place(enemies.formation[actor.formationSlot - 1]);
      actor.formationSlotReserved = false;
    };
    auto allocateFormation = [&] {
      if (inputs.reuseVacantFormationSlots && enemies.nextFormationSlot >= enemies.formation.size()) {
        for (unsigned candidate = 0; candidate < enemies.formation.size(); ++candidate) {
          if (enemies.formation[candidate].visible) continue;
          const bool reserved = std::any_of(enemies.incoming.begin(), enemies.incoming.end(),
              [&](const auto& other) {
                return &other != &actor && other.formationSlotReserved &&
                    other.formationSlot == candidate + 1;
              });
          if (reserved) continue;
          actor.formationSlot = candidate + 1;
          actor.formationSlotReserved = true;
          return;
        }
        throw std::overflow_error("No empty, unreserved Gyruss formation slot is available");
      }
      actor.formationSlot = ++enemies.nextFormationSlot;
      actor.formationSlotReserved = true;
    };
    if (cursor & 128) {
      if (!inputs.deathCountdown) ++cursor;
      else { allocateFormation(); arrive(); }
      continue;
    }
    bool draw = false;
    bool consume = cursor == 0;
    if (!consume) {
      if (actor.hit) {
        events.award(2);
        if (enemies.remainingBonusEnemies) --enemies.remainingBonusEnemies;
        if (paths.forcedSprite) events.sounds.push_back(5);
        if (paths.incomingCount == 1) {
          if (inputs.returnToFormation && !enemies.formationCount) paths.scoreChain = 130;
          if (paths.scoreChain & 128) {
            const unsigned bonus = paths.scoreChain & 127;
            // $8FF4..$8FFB repeats the ten-unit call; zero wraps to 256 calls.
            for (unsigned i = 0; i < (bonus ? bonus : 256); ++i) events.award(10);
            events.bonuses.push_back({actor.angle, actor.depth, static_cast<std::uint8_t>(bonus - 1), true});
            events.sounds.push_back(7);
            if (inputs.normalStage && bonus < 8) ++paths.scoreChain;
          }
        }
        remove();
        continue;
      }
      if (phase & 128) consume = true;
      else if (phase & 64) {
        // Marker-directed outward dive toward the player's current angle.
        if (inputs.levelNumber < 7) {
          phase ^= 1;
          if (!(phase & 1)) draw = true;
        }
        if (!draw) {
          actor.depth -= 2;
          if (actor.depth & 128) { remove(); continue; }
          const int delta = steeringDelta(actor.angle, inputs.playerAngle);
          actor.spriteType = static_cast<std::uint8_t>(10 - delta);
          if (delta) {
            actor.angle = static_cast<std::uint8_t>((actor.angle + delta) & 127);
            actor.heading = static_cast<std::uint8_t>(((actor.angle >> 3) - delta) & 15);
          }
          draw = true;
        }
      } else if (phase < 42) {
        phase += 2;
        actor.depth = phase;
        draw = true;
      } else {
        if (!actor.formationSlot || actor.formationSlot > enemies.formation.size())
          throw std::out_of_range("Original formation target outside observed range");
        const auto target = enemies.formation[actor.formationSlot - 1].angle;
        if (actor.angle == target) { arrive(); continue; }
        const int delta = ((actor.angle - target) & 64) ? 2 : -2;
        actor.spriteType = static_cast<std::uint8_t>(10 - delta);
        actor.angle = static_cast<std::uint8_t>((actor.angle + delta) & 127);
        if (actor.angle == target) { arrive(); continue; }
        actor.heading = actor.angle >> 3;
        draw = true;
      }
    }
    while (consume) {
      if (cursor >= 128) throw std::out_of_range("Original enemy path cursor overflow");
      const auto point = paths.tracks[index & 1][cursor++];
      if (point.style == 255) {
        paths.collisionMask = 3;
        paths.scoreChain &= 127;
        if (!inputs.normalStage) { remove(); break; }
        actor.depth += 2;
        phase = actor.depth;
        allocateFormation();
        actor.spriteType = 6;
        draw = true;
        break;
      }
      if (inputs.deathCountdown) {
        if (actor.spriteType == 8) actor.angle = (actor.angle + 2) & 127;
        else if (actor.spriteType == 12) actor.angle = (actor.angle - 2) & 127;
        actor.depth -= 2;
        if (actor.depth & 128) { remove(); break; }
        draw = true;
        break;
      }
      if (!(point.style & 15)) {
        if ((point.style >> 4) != index) continue;
        paths.collisionMask = 3;
        phase = 64;
        if (inputs.levelNumber < 7) phase ^= 1;
        actor.depth -= 2;
        if (actor.depth & 128) { remove(); break; }
        const int delta = steeringDelta(actor.angle, inputs.playerAngle);
        actor.spriteType = static_cast<std::uint8_t>(10 - delta);
        if (delta) {
          actor.angle = static_cast<std::uint8_t>((actor.angle + delta) & 127);
          actor.heading = static_cast<std::uint8_t>(((actor.angle >> 3) - delta) & 15);
        }
        draw = true;
        break;
      }
      actor.spriteType = point.style & 15;
      actor.heading = point.style >> 4;
      actor.depth = point.depth;
      actor.angle = point.angle;
      draw = true;
      break;
    }
    if (draw) {
      if (!place(actor)) {
        if (!inputs.normalStage) remove();
        else if (inputs.returnToFormation) {
          if (!inputs.randomByte) throw std::invalid_argument("Original clipping decision needs a random byte source");
          if (!(inputs.randomByte() & 3)) remove();
          else arrive();
        } else { allocateFormation(); arrive(); }
      } else if (inputs.afterIncomingDraw) inputs.afterIncomingDraw(index);
    }
  }
  return events;
}

EnemyEvents updateEnemyEntries(EnemyPaths& paths, EnemyState& enemies,
                               const EnemyInputs& inputs) {
  EnemyEvents events;
  if (paths.appearanceDelay) {
    --paths.appearanceDelay;
    if (paths.appearanceDelay) return events;
    events.sounds.push_back(2);
  }
  if (--paths.evenTimer == 0) append(events, updateIncomingLane(paths, enemies, 0, inputs));
  if (--paths.oddTimer == 0) append(events, updateIncomingLane(paths, enemies, 1, inputs));
  return events;
}

void prepareWave(EnemyPaths& paths, const WaveSettings& settings) {
  if (settings.stageOffset > 40 || (settings.stageOffset & 7) ||
      settings.waveIndex > 4 || (settings.waveIndex == 4 && settings.randomWave >= 24))
    throw std::out_of_range("Unsupported original Gyruss wave settings");
  paths.scoreChain |= 128;
  paths.incomingCount = 8;
  paths.phases.fill(255);
  paths.forcedSprite = 0;
  unsigned descriptor = settings.waveIndex == 4 ? settings.randomWave :
      ((settings.stageOffset >> 1) | settings.waveIndex);
  if (!settings.normalStage) {
    constexpr std::array<std::uint8_t, 6> forced{14, 4, 2, 2, 4, 2};
    if ((settings.waveIndex & 1) == 0) paths.forcedSprite = forced[settings.stageOffset >> 3];
    descriptor += 24;
  }
  descriptor *= 4;
  std::array<std::uint8_t, 2> angles{
      static_cast<std::uint8_t>(paths.descriptors[descriptor + 1] & 126),
      static_cast<std::uint8_t>(paths.descriptors[descriptor] & 126)};
  // The Atari source uses read-modify-write LSR on its descriptor table.
  // Preserve this in the writable ATR profile; cartridge profile ignores writes.
  paths.phases[8] = (paths.descriptors[descriptor] & 1) ? 128 : 0;
  paths.phases[9] = (paths.descriptors[descriptor + 1] & 1) ? 128 : 0;
  paths.incomingCount += (paths.phases[8] != 0) + (paths.phases[9] != 0);
  if (settings.writableDescriptors) {
    paths.descriptors[descriptor] >>= 1;
    paths.descriptors[descriptor + 1] >>= 1;
  }
  const auto flags = paths.descriptors[descriptor + 2];
  const auto pathConfig = paths.descriptors[descriptor + 3];
  if ((flags & 63) >= enemy_data::depths.size() || (pathConfig & 31) >= enemy_data::offsets.size())
    throw std::out_of_range("Invalid original enemy descriptor");
  std::uint8_t depth = enemy_data::depths[flags & 63];
  for (unsigned i = 0; i < paths.cursors.size(); ++i)
    paths.cursors[i] = static_cast<std::uint8_t>(255 - i * (pathConfig >> 5));
  constexpr std::array<int, 8> depthDelta{2, -2, 0, 2, -2, 0, 2, -2};
  constexpr std::array<int, 8> angleDelta{0, 0, -2, -2, -2, 2, 2, 2};
  constexpr std::array<int, 8> headingDelta{0, 0, 0, -2, 2, 0, 2, -2};
  unsigned count = 0;
  unsigned cursor = enemy_data::offsets[pathConfig & 31];
  for (;;) {
    if (cursor >= enemy_data::commands.size()) throw std::out_of_range("Unterminated original enemy path");
    const auto command = enemy_data::commands[cursor++];
    if (command == 0) break;
    const unsigned operation = command >> 4;
    const unsigned repetitions = operation >= 8 ? 1 : (command & 15);
    for (unsigned repeat = 0; repeat < repetitions; ++repeat) {
      if (count >= 127) throw std::out_of_range("Original enemy path exceeds track capacity");
      if (operation < 8) depth = static_cast<std::uint8_t>(depth + depthDelta[operation]);
      for (unsigned lane = 0; lane < 2; ++lane) {
        const bool mirrored = flags & (lane == 0 ? 64 : 128);
        auto& point = paths.tracks[lane][count];
        std::uint8_t style;
        if (operation >= 8) {
          style = static_cast<std::uint8_t>((command & 15) << 4);
        } else {
          const int sign = mirrored ? -1 : 1;
          angles[lane] = static_cast<std::uint8_t>((angles[lane] + sign * angleDelta[operation]) & 127);
          if (paths.forcedSprite || operation < 2) {
            style = paths.forcedSprite ? paths.forcedSprite : (operation == 0 ? 6 : 10);
          } else {
            const auto heading = ((angles[lane] >> 3) + sign * headingDelta[operation]) & 15;
            const auto sprite = (operation < 5 ? 12 : 8) ^ (mirrored ? 4 : 0);
            style = static_cast<std::uint8_t>((heading << 4) | sprite);
          }
        }
        point = {depth, angles[lane], style};
      }
      ++count;
    }
  }
  paths.programLength = static_cast<std::uint8_t>(count);
  paths.tracks[0][count].style = paths.tracks[1][count].style = 255;
  paths.nextWave = settings.waveIndex + 1;
  paths.appearanceDelay = 60;
  paths.cadence = settings.stageOffset >= 40 ? 2 : 3;
  paths.evenTimer = paths.cadence;
  paths.oddTimer = paths.cadence >> 1;
  paths.collisionMask = settings.normalStage ? 2 : 3;
  ++paths.preparedCount;
}
} // namespace gyrus::game
