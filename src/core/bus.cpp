#include "core/bus.h"

#include <utility>

namespace ct10::core {

Bus::Bus(std::string name) : name_(std::move(name)) {}

const std::string& Bus::name() const { return name_; }

bool Bus::driven() const { return driven_; }

bool Bus::complemented() const { return complemented_; }

uint16_t Bus::value() const { return value_; }

void Bus::Drive(uint16_t value, bool complemented) {
  value_ = value;
  driven_ = true;
  complemented_ = complemented;
}

void Bus::Clear() {
  value_ = 0;
  driven_ = false;
  complemented_ = false;
}

}  // namespace ct10::core
