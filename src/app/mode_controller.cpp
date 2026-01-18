#include "app/mode_controller.h"

namespace ct10::app {

void ModeController::SetMode(RunMode mode) { mode_ = mode; }

RunMode ModeController::mode() const { return mode_; }

bool ModeController::IsHalted() const { return mode_ == RunMode::Halted; }

}  // namespace ct10::app
