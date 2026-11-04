#pragma once

#include "Enemies.hpp"

namespace gyrus::game {

struct FormationControl {
  std::array<std::uint8_t, 10> attackState{}; // original $DC..$E5
  std::uint8_t randomTurnMask = 31; // $E7
  std::uint8_t launchMask = 127; // $E9
  std::uint8_t maxIncoming = 2; // $E8
  std::uint8_t frameCounter = 0; // $56, incremented every gameplay update
  std::uint8_t gridPhase = 0; // $5B, alternates rotation and depth oscillation
};

FormationControl formationControl(std::uint8_t levelNumber);
unsigned attemptFormationLaunch(EnemyPaths&, EnemyState&, FormationControl&, const EnemyInputs&);
void updateFormationProgram(EnemyPaths&, EnemyState&, FormationControl&, unsigned actorIndex,
                            const EnemyInputs&);
EnemyEvents updateEnemyEntriesWithFormation(EnemyPaths&, EnemyState&, FormationControl&,
                                            const EnemyInputs&);
EnemyEvents updateIncomingLaneWithFormation(EnemyPaths&, EnemyState&, FormationControl&,
                                            unsigned parity, const EnemyInputs&);
EnemyEvents updateFormationGrid(EnemyState&, FormationControl&);

} // namespace gyrus::game
