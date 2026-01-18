#include "app/golden_program.h"
#include "app/main_loop.h"
#include "app/mode_controller.h"
#include "core/execution_engine.h"
#include "core/machine_state.h"
#include "core/timing_engine.h"

int main() {
  ct10::core::MachineState state;
  ct10::core::TimingEngine timing;
  ct10::core::ExecutionEngine execution;
  ct10::app::ModeController mode;
  ct10::app::MainLoop loop;

  ct10::app::LoadGoldenProgram(state);
  timing.Reset(state.timing);

  return loop.Run(state, timing, execution, mode, ct10::app::LoadGoldenProgram);
}
