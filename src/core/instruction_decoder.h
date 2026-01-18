#pragma once

#include <cstdint>
#include <string_view>

namespace ct10::core {

enum class AddressingMode : uint8_t {
  Direct,
  Indexed,
  Immediate,
  Paged,
  Unknown,
};

struct Instruction {
  uint8_t opcode = 0;
  std::string_view mnemonic = "???";
  AddressingMode addressing = AddressingMode::Unknown;
  bool is_halt = false;
};

class InstructionDecoder {
 public:
  Instruction Decode(uint8_t opcode) const;
};

}  // namespace ct10::core
