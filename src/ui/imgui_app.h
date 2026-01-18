#pragma once

#include <functional>

#include "app/mode_controller.h"
#include "core/execution_engine.h"
#include "core/machine_state.h"
#include "core/timing_engine.h"

namespace ct10::ui {

class ImGuiApp {
 public:
  using ResetHook = std::function<void(core::MachineState&)>;

  int Run(core::MachineState& state,
          core::TimingEngine& timing,
          core::ExecutionEngine& execution,
          app::ModeController& mode,
          const ResetHook& reset_hook) const;
};

}  // namespace ct10::ui
