#pragma once

#include <cstdint>
#include <vector>

#include "core/microcode.h"

namespace ct10::core {

class MicrocodeTable {
 public:
  static const std::vector<MicroOpStep>& Acquisition();
  static const std::vector<MicroOpStep>& Execution(uint8_t opcode);
};

}  // namespace ct10::core
