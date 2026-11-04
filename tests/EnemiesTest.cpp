#include <Game/Enemies.hpp>

#include <iostream>
#include <stdexcept>

namespace {
void require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}
}

int main() {
  using namespace gyrus::game;
  EnemyPaths paths;
  prepareWave(paths, {});
  require(paths.incomingCount == 9, "first wave has eight base enemies plus one descriptor flag");
  require(paths.nextWave == 1 && paths.programLength == 54, "first wave path length changed");
  require(paths.cursors[0] == 255 && paths.cursors[9] == 237, "staggered entry delays changed");
  require(paths.tracks[0][0].angle == 48 && paths.tracks[1][0].angle == 80 &&
              paths.tracks[0][0].depth == 50 && paths.tracks[0][0].style == 10,
          "first original path point changed");
  require(paths.tracks[0][54].style == 255 && paths.tracks[1][54].style == 255,
          "both tracks need the original end marker");
  require(paths.descriptors[0] == 0x28 && paths.descriptors[1] == 0x18,
          "ATR writable descriptor behavior changed");
  EnemyPaths cartridge;
  WaveSettings settings;
  settings.writableDescriptors = false;
  prepareWave(cartridge, settings);
  require(cartridge.descriptors[0] == 0x51, "read-only cartridge profile mutated descriptor");
  settings.stageOffset = 40;
  prepareWave(cartridge, settings);
  require(cartridge.cadence == 2 && cartridge.evenTimer == 2 && cartridge.oddTimer == 1,
          "last stage must use original faster path cadence");
  bool rejected = false;
  try { settings.stageOffset = 7; prepareWave(cartridge, settings); }
  catch (const std::out_of_range&) { rejected = true; }
  require(rejected, "unsupported stage offset must be rejected");
  EnemyPaths entering;
  EnemyState actors;
  prepareWave(entering, {});
  updateIncomingLane(entering, actors, 0, {});
  require(entering.cursors[0] == 0 && !actors.incoming[0].visible,
          "first update consumes the original initial delay");
  updateIncomingLane(entering, actors, 0, {});
  require(entering.cursors[0] == 1 && actors.incoming[0].visible &&
              actors.incoming[0].angle == 48 && actors.incoming[0].depth == 50,
          "first enemy must use the decoded original path point");
  require(actors.formation[0].angle == 116 && actors.formation[35].depth == 44,
          "formation layout must match captured Atari startup");
  EnemyPaths overflow;
  EnemyState crowded;
  overflow.nextWave = 5;
  overflow.incomingCount = 2;
  overflow.phases[0] = 255;
  overflow.phases[1] = 42;
  overflow.tracks[0][0].style = 255;
  crowded.nextFormationSlot = 36;
  crowded.incoming[0].depth = 38;
  crowded.incoming[1].formationSlot = 1;
  crowded.incoming[1].formationSlotReserved = true;
  EnemyInputs repaired;
  repaired.reuseVacantFormationSlots = true;
  const auto initialOverflow = overflow;
  const auto initialCrowded = crowded;
  updateIncomingLane(overflow, crowded, 0, repaired);
  require(crowded.nextFormationSlot == 36 && crowded.incoming[0].formationSlot == 2 &&
              crowded.incoming[0].formationSlotReserved,
          "overflow repair must reuse an empty, unreserved formation slot");
  crowded.incoming[0].hit = true;
  updateIncomingLane(overflow, crowded, 0, repaired);
  require(!crowded.incoming[0].formationSlotReserved, "removed actor must release formation reservation");
  auto strictPaths = initialOverflow;
  auto strictEnemies = initialCrowded;
  updateIncomingLane(strictPaths, strictEnemies, 0, {});
  require(strictEnemies.nextFormationSlot == 37 && strictEnemies.incoming[0].formationSlot == 37,
          "strict profile must retain original sequential allocation");
  auto lastPaths = initialOverflow;
  auto lastEnemies = initialCrowded;
  lastEnemies.nextFormationSlot = 35;
  updateIncomingLane(lastPaths, lastEnemies, 0, repaired);
  require(lastEnemies.nextFormationSlot == 36 && lastEnemies.incoming[0].formationSlot == 36,
          "repair must not alter the first36 allocations");
  auto fullPaths = initialOverflow;
  auto fullEnemies = initialCrowded;
  for (auto& actor : fullEnemies.formation) actor.visible = true;
  bool exhausted = false;
  try { updateIncomingLane(fullPaths, fullEnemies, 0, repaired); }
  catch (const std::overflow_error&) { exhausted = true; }
  require(exhausted, "full formation must report exhaustion explicitly");
  std::cout << "Enemy wave regression checks passed\n";
}
