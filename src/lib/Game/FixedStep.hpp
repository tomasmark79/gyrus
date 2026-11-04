#pragma once

#include <algorithm>
#include <cmath>

namespace gyrus::game {
class FixedStep {
public:
  // PAL adapter cadence. Rendering may run at any refresh rate. Limit catch-up
  // after a suspended tab so stale input cannot advance an unseen game.
  template<class Callback> void advance(double elapsedSeconds, Callback tick) {
    if (!std::isfinite(elapsedSeconds) || elapsedSeconds < 0) return;
    remainder_ += std::min(elapsedSeconds, 0.1);
    while (remainder_ + 1e-12 >= 0.02) {
      remainder_ = std::max(0.0, remainder_ - 0.02);
      tick();
    }
  }
private:
  double remainder_{};
};
}
