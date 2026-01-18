#include "app/main_loop.h"

#include "ui/imgui_app.h"

namespace ct10::app {

int MainLoop::Run(core::MachineState& state,
                  core::TimingEngine& timing,
                  core::ExecutionEngine& execution,
                  ModeController& mode,
                  const ResetHook& reset_hook) const {
  ui::ImGuiApp app;
  return app.Run(state, timing, execution, mode, reset_hook);
}

}  // namespace ct10::app
