#include "app/golden_program.h"

#include "core/machine_state.h"

namespace ct10::app {

void LoadGoldenProgram(core::MachineState& state) {
  state.Reset();
  state.memory.Clear();

  const uint8_t program[] = {
      0x20, 0x20,  // LDA 0x20
      0x60, 0x20,  // ADD 0x20
      0x60, 0x23,  // ADD 0x23
      0x68, 0x22,  // SUB 0x22
      0x48, 0x21,  // STA 0x21
      0x98, 0x00,  // BST 0x00
  };

  for (uint16_t i = 0; i < sizeof(program); ++i) {
    state.memory.Write(i, program[i]);
  }

  state.memory.Write(0x20, 0x04);
  state.memory.Write(0x21, 0x00);
  state.memory.Write(0x22, 0x01);
  state.memory.Write(0x23, 0x05);
  state.par.Load(0x000);
}

}  // namespace ct10::app
