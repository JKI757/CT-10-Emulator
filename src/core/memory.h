#pragma once

#include <array>
#include <cstdint>

namespace ct10::core {

class Memory {
 public:
  static constexpr uint16_t kSize = 1024;
  static constexpr uint16_t kAddressMask = kSize - 1;

  uint8_t Read(uint16_t address) const;
  void Write(uint16_t address, uint8_t value);
  void Clear();

  const std::array<uint8_t, kSize>& cells() const;

 private:
  std::array<uint8_t, kSize> cells_{};
};

}  // namespace ct10::core
