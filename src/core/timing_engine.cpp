#include "core/timing_engine.h"

namespace ct10::core {

void TimingEngine::Reset(TimingState& state) const {
  state.distributor = 0;
  state.phase = ClockPhase::CP1;
  state.acquisition = true;
}

void TimingEngine::Advance(TimingState& state) const {
  switch (state.phase) {
    case ClockPhase::CP1:
      state.phase = ClockPhase::CP2;
      break;
    case ClockPhase::CP2:
      state.phase = ClockPhase::CP3;
      break;
    case ClockPhase::CP3:
      state.phase = ClockPhase::CP1;
      state.distributor = static_cast<uint8_t>((state.distributor + 1) & 0x0F);
      if (state.distributor == 0) {
        state.acquisition = !state.acquisition;
      }
      break;
  }
}

void TimingEngine::set_speed_multiplier(double speed_multiplier) {
  if (speed_multiplier > 0.0) {
    speed_multiplier_ = speed_multiplier;
  }
}

double TimingEngine::speed_multiplier() const { return speed_multiplier_; }

}  // namespace ct10::core
