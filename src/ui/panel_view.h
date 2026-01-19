#pragma once

#include "core/machine_state.h"

struct ImFont;

namespace ct10::ui {

class PanelView {
 public:
  PanelView(ImFont* display_font = nullptr, ImFont* input_font = nullptr);
  void Draw(core::MachineState& state) const;

 private:
  ImFont* display_font_ = nullptr;
  ImFont* input_font_ = nullptr;
};

}  // namespace ct10::ui
