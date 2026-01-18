#pragma once

#include <cstdint>

namespace ct10::core {

enum class ClockPhase : uint8_t {
  CP1 = 1,
  CP2 = 2,
  CP3 = 3,
};

struct TimingState {
  uint8_t distributor = 0;
  ClockPhase phase = ClockPhase::CP1;
  bool acquisition = true;
};

class TimingEngine {
 public:
  void Reset(TimingState& state) const;
  void Advance(TimingState& state) const;

  void set_speed_multiplier(double speed_multiplier);
  double speed_multiplier() const;

 private:
  double speed_multiplier_ = 1.0;
};

}  // namespace ct10::core
