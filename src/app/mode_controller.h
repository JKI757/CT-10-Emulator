#pragma once

namespace ct10::app {

enum class RunMode {
  Halted,
  Continuous,
  StepDistributor,
  StepClock,
};

class ModeController {
 public:
  void SetMode(RunMode mode);
  RunMode mode() const;
  bool IsHalted() const;

 private:
  RunMode mode_ = RunMode::Halted;
};

}  // namespace ct10::app
