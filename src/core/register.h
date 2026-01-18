#pragma once

#include <cstdint>
#include <string>

namespace ct10::core {

class Register {
 public:
  Register(std::string name, uint8_t width);

  const std::string& name() const;
  uint8_t width() const;
  uint16_t value() const;

  void Load(uint16_t value);
  void Clear();
  void Increment();
  void Decrement();

 private:
  static uint16_t MaskForWidth(uint8_t width);

  std::string name_;
  uint8_t width_ = 0;
  uint16_t mask_ = 0;
  uint16_t value_ = 0;
};

}  // namespace ct10::core
