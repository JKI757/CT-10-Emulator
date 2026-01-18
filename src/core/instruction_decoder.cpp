#include "core/instruction_decoder.h"

namespace ct10::core {

Instruction InstructionDecoder::Decode(uint8_t opcode) const {
  switch (opcode) {
    case 0x00:
      return {opcode, "SST", AddressingMode::Immediate, false};
    case 0x01:
      return {opcode, "LCI", AddressingMode::Immediate, false};
    case 0x02:
      return {opcode, "LAI", AddressingMode::Immediate, false};
    case 0x03:
      return {opcode, "INX", AddressingMode::Immediate, false};
    case 0x08:
      return {opcode, "SKI", AddressingMode::Immediate, false};
    case 0x09:
      return {opcode, "SKS", AddressingMode::Immediate, false};
    case 0x0A:
      return {opcode, "SKF", AddressingMode::Immediate, false};
    case 0x0B:
      return {opcode, "SLA", AddressingMode::Immediate, false};
    case 0x10:
      return {opcode, "SRA", AddressingMode::Immediate, false};
    case 0x11:
      return {opcode, "OCD", AddressingMode::Immediate, false};
    case 0x12:
      return {opcode, "LXI", AddressingMode::Immediate, false};
    case 0x13:
      return {opcode, "SLL", AddressingMode::Immediate, false};
    case 0x18:
      return {opcode, "SRL", AddressingMode::Immediate, false};
    case 0x19:
      return {opcode, "AND", AddressingMode::Immediate, false};
    case 0x1A:
      return {opcode, "IOR", AddressingMode::Immediate, false};
    case 0x1B:
      return {opcode, "XOR", AddressingMode::Immediate, false};
    case 0x28:
      return {opcode, "FLC", AddressingMode::Immediate, false};
    case 0xF8:
      return {opcode, "FLS", AddressingMode::Immediate, false};
    default:
      break;
  }

  uint8_t base = static_cast<uint8_t>(opcode & 0xF8);
  switch (base) {
    case 0x20:
      return {opcode, "LDA", AddressingMode::Paged, false};
    case 0x30:
      return {opcode, "LCC", AddressingMode::Paged, false};
    case 0x38:
      return {opcode, "LAN", AddressingMode::Paged, false};
    case 0x40:
      return {opcode, "LDQ", AddressingMode::Paged, false};
    case 0x48:
      return {opcode, "STA", AddressingMode::Paged, false};
    case 0x50:
      return {opcode, "STX", AddressingMode::Paged, false};
    case 0x58:
      return {opcode, "STQ", AddressingMode::Paged, false};
    case 0x60:
      return {opcode, "ADD", AddressingMode::Paged, false};
    case 0x68:
      return {opcode, "SUB", AddressingMode::Paged, false};
    case 0x70:
      return {opcode, "MPY", AddressingMode::Paged, false};
    case 0x78:
      return {opcode, "DIV", AddressingMode::Paged, false};
    case 0x80:
      return {opcode, "RAO", AddressingMode::Paged, false};
    case 0x88:
      return {opcode, "RSO", AddressingMode::Paged, false};
    case 0x90:
      return {opcode, "BUN", AddressingMode::Paged, false};
    case 0x98:
      return {opcode, "BST", AddressingMode::Paged, true};
    case 0xA0:
      return {opcode, "BSB", AddressingMode::Paged, false};
    case 0xA8:
      return {opcode, "BPS", AddressingMode::Paged, false};
    case 0xB0:
      return {opcode, "BZE", AddressingMode::Paged, false};
    case 0xB8:
      return {opcode, "BNG", AddressingMode::Paged, false};
    case 0xC0:
      return {opcode, "BNC", AddressingMode::Paged, false};
    case 0xC8:
      return {opcode, "BXZ", AddressingMode::Paged, false};
    case 0xD0:
      return {opcode, "WDB", AddressingMode::Paged, false};
    case 0xD8:
      return {opcode, "MNO", AddressingMode::Paged, false};
    case 0xE0:
      return {opcode, "RDB", AddressingMode::Paged, false};
    case 0xE8:
      return {opcode, "RDI", AddressingMode::Paged, false};
    case 0xF0:
      return {opcode, "MNI", AddressingMode::Paged, false};
    default:
      return {opcode, "???", AddressingMode::Unknown, false};
  }
}

}  // namespace ct10::core
