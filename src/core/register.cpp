#include "core/register.h"

#include <utility>

namespace ct10::core {

Register::Register(std::string name, uint8_t width)
    : name_(std::move(name)), width_(width), mask_(MaskForWidth(width)) {}

const std::string& Register::name() const { return name_; }

uint8_t Register::width() const { return width_; }

uint16_t Register::value() const { return value_; }

void Register::Load(uint16_t value) { value_ = value & mask_; }

void Register::Clear() { value_ = 0; }

void Register::Increment() { value_ = (value_ + 1) & mask_; }

void Register::Decrement() { value_ = (value_ - 1) & mask_; }

uint16_t Register::MaskForWidth(uint8_t width) {
  if (width >= 16) {
    return 0xFFFFu;
  }
  if (width == 0) {
    return 0;
  }
  return static_cast<uint16_t>((1u << width) - 1u);
}

}  // namespace ct10::core
