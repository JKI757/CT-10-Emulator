#pragma once

#include <cstdint>

namespace ct10::core {
class MachineState;
}

namespace ct10::app {

constexpr uint16_t kGoldenProgramResultAddress = 0x21;
constexpr uint8_t kGoldenProgramExpectedValue = 0x0C;

void LoadGoldenProgram(core::MachineState& state);

}  // namespace ct10::app
