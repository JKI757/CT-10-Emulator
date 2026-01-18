#include "core/memory.h"

namespace ct10::core {

uint8_t Memory::Read(uint16_t address) const {
  return cells_[address & kAddressMask];
}

void Memory::Write(uint16_t address, uint8_t value) {
  cells_[address & kAddressMask] = value;
}

void Memory::Clear() {
  cells_.fill(0);
}

const std::array<uint8_t, Memory::kSize>& Memory::cells() const {
  return cells_;
}

}  // namespace ct10::core
