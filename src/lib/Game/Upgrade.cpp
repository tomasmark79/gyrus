#include "Upgrade.hpp"
#include "EnemyData.hpp"

namespace gyrus::game {
UpgradeState::UpgradeState() {
    for (auto& part : particles) part.actor.spriteType = 0x22;
}
namespace {
constexpr std::array<int,6> initialAngles{-40,40,-26,26,0,0}; // $AC7D.
constexpr std::array<std::uint8_t,6> initialDepths{26,26,32,32,42,18}; // $AC83.
constexpr std::array<int,6> angleSteps{2,-2,1,-1,0,0}; // $AC89.
constexpr std::array<int,6> depthSteps{0,0,0,0,-1,1}; // $AC8F.

void hide(EnemyActor& actor) {
    actor.visible=false; actor.hit=false; actor.width=0;
}

bool draw(EnemyActor& actor, std::uint8_t descriptor) {
    if(actor.depth>=64 || descriptor>=58) { hide(actor); return false; }
    const auto point=project(actor.angle,actor.depth);
    if(point.clipped || point.x>=160 || point.y>=96) { hide(actor); return false; }
    actor.position=point;
    const auto& profile=enemy_data::spriteProfiles[descriptor/2];
    const auto packed=profile[1+(actor.depth>>4)];
    actor.width=((actor.depth&8)?packed:(packed>>4))&15;
    if(profile[0]<128) actor.heading=profile[0]?actor.angle>>(profile[0]+2):0;
    const unsigned left=point.x>actor.width/2 ? point.x-actor.width/2:0;
    const unsigned top=point.y>actor.width/2 ? point.y-actor.width/2:0;
    if(!actor.width || left+actor.width>=160 || top+actor.width>=96) { hide(actor); return false; }
    actor.visible=true;
    actor.renderedDepth=actor.depth;
    actor.renderedSpriteType=descriptor;
    actor.renderedHeading=actor.heading;
    return true;
}

void releaseCollectibles(UpgradeState& state,const UpgradeInputs& inputs) {
    for(unsigned i=2;i<6;++i) state.particles[i].actor.depth=0;
    for(unsigned i=0;i<2;++i) {
        state.particles[i].actor.depth=inputs.playerDepth;
        state.particles[i].actor.heading=1;
    }
    // $8A7C..$8A88: ADC inherits carry=1; SBC inherits ADC's overflow carry.
    const unsigned sum=static_cast<unsigned>(inputs.playerAngle)+13;
    const auto first=static_cast<std::uint8_t>(sum)&126;
    state.particles[0].actor.angle=first;
    state.particles[1].actor.angle=static_cast<std::uint8_t>(first-24-(sum<=255?1:0))&126;
    state.phase=0xc2;
}
} // namespace

UpgradeSpawnResult spawnUpgrade(UpgradeState& state,const UpgradeInputs& inputs) {
    if(state.targetPhase || !inputs.returnToFormation) return UpgradeSpawnResult::none;
    if(inputs.formationCount<3) {
        state.targetCount=0;
        --state.targetPhase;
        return UpgradeSpawnResult::abandoned;
    }
    if(inputs.formationCount>=6) {
        if(inputs.tick&12) return UpgradeSpawnResult::none;
        if(--state.spawnDelay) return UpgradeSpawnResult::none;
    }
    state.targets[0].spriteType=4;
    state.targets[1].spriteType=state.dualShotLevel?4:16;
    state.targets[2].spriteType=4;
    for(unsigned i=0;i<6;++i) {
        auto& actor=state.particles[i].actor;
        actor.angle=static_cast<std::uint8_t>(inputs.playerAngle+initialAngles[i])&126;
        actor.depth=initialDepths[i];
        actor.heading=0;
    }
    state.phase=0x8c;
    for(unsigned i=0;i<3;++i) {
        state.targets[i].angle=static_cast<std::uint8_t>(inputs.playerAngle+12-12*static_cast<int>(i))&126;
        state.targets[i].depth=50;
    }
    ++state.targetPhase;
    return UpgradeSpawnResult::spawned;
}

EnemyEvents updateUpgradeTargets(UpgradeState& state,const UpgradeInputs& inputs) {
    EnemyEvents events;
    if((inputs.tick&7) || !state.targetCount || !state.targetPhase) return events;
    const auto previous=state.targetPhase++;
    if(previous<7) {
        if((previous&1) && !(previous&2) && (previous>>2)<3)
            state.forcedShooter=static_cast<std::uint8_t>((previous>>2)+1);
        return events;
    }
    if(previous==7) {
        for(auto& target:state.targets) {
            target.width=28;
            target.visible=true;
            target.hit=false;
            target.depth=28;
            target.heading=1;
        }
        state.scoreChain=0;
        state.animation=25;
    } else {
        for(unsigned i=0;i<3;++i) {
            auto& target=state.targets[i];
            if(!target.visible || !target.hit) continue;
            hide(target);
            --state.targetCount;
            if(i==1 && !state.dualShotLevel) releaseCollectibles(state,inputs);
            events.award(10);
            const auto label=target.spriteType==16?0:static_cast<std::uint8_t>((state.scoreChain>>3)+1);
            // CMP #$10 gives carry1 for the middle sprite; otherwise three
            // LSRs leave old scoreChain bit2 in carry at $8AB0.
            const bool popupCarry=target.spriteType==16 || (state.scoreChain&4);
            events.bonuses.push_back({target.angle,target.depth,static_cast<std::uint8_t>(label),popupCarry});
            if(target.spriteType==16) {
                events.sounds.push_back(9);
            } else {
                events.award(state.scoreChain);
                events.award(10);
                state.scoreChain=static_cast<std::uint8_t>(state.scoreChain+10);
                events.sounds.push_back(5);
            }
        }
        state.animation=static_cast<std::uint8_t>((state.animation<<2)|(state.animation>>6));
        for(auto& target:state.targets) target.heading=state.animation&3;
        int angleDelta=0,depthDelta=0;
        if(inputs.deathCountdown || (state.targetPhase&128)) {
            depthDelta=-4;
        } else {
            const int direction=(state.targetPhase&2)?1:-1;
            if(state.targetPhase&1) {
                depthDelta=2*direction;
            } else {
                const int magnitude=inputs.levelRank==0?6:inputs.levelRank<4?2*inputs.levelRank:8;
                angleDelta=direction*magnitude;
            }
        }
        for(auto& target:state.targets) {
            target.angle=static_cast<std::uint8_t>(target.angle+angleDelta)&126;
            target.depth=static_cast<std::uint8_t>(target.depth+depthDelta);
            if((target.depth&128) && target.visible) { hide(target); --state.targetCount; }
        }
    }
    for(auto& target:state.targets) {
        if(target.visible && !draw(target,target.spriteType)) --state.targetCount;
    }
    return events;
}

bool updateUpgradeParticles(UpgradeState& state,const UpgradeInputs& inputs) {
    const bool fast=(state.phase&64)!=0;
    if((inputs.tick&(fast?1:3)) || !state.phase) return false;
    // $871B loads the descriptor from slot28 once, shared by all six particles.
    const auto descriptor=state.particles[5].actor.spriteType;
    for(int i=5;i>=0;--i) {
        auto& particle=state.particles[static_cast<unsigned>(i)];
        auto& actor=particle.actor;
        if(!actor.depth) continue;
        if(!(state.phase&128)) {
            particle.drawn=false;
            if(state.phase==1) continue;
        }
        particle.drawn=draw(actor,descriptor);
        actor.angle=static_cast<std::uint8_t>(actor.angle+angleSteps[static_cast<unsigned>(i)]*(fast?2:1))&126;
        actor.depth=static_cast<std::uint8_t>(actor.depth+depthSteps[static_cast<unsigned>(i)]);
    }
    state.phase&=127;
    if(state.phase&64) {
        for(unsigned i=0;i<3;++i) {
            auto& particle=state.particles[i];
            if(!particle.actor.depth || ((particle.actor.angle^inputs.playerAngle)&0xf8)) continue;
            particle.actor.depth=0;
            particle.drawn=false;
            --state.phase;
            if((state.phase&3)==0) {
                ++state.dualShotLevel;
                state.phase=0;
                return true;
            }
        }
    } else {
        --state.phase;
    }
    return false;
}
} // namespace gyrus::game
