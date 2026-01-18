#pragma once

#include "core/machine_state.h"

namespace ct10::ui {

class DebugPane {
 public:
  void Draw(const core::MachineState& state, float top_offset) const;
};

}  // namespace ct10::ui
