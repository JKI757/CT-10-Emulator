#pragma once

#include "core/machine_state.h"

namespace ct10::ui {

class PanelView {
 public:
  void Draw(core::MachineState& state) const;
};

}  // namespace ct10::ui
