#pragma once

#include "core/machine_state.h"
#include "core/microcode.h"

namespace ct10::core {

class ExecutionEngine {
 public:
  void Step(MachineState& state) const;

 private:
  void ExecuteMicroOp(MicroOp op, MachineState& state) const;
};

}  // namespace ct10::core
