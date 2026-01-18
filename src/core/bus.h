#pragma once

#include <cstdint>
#include <string>

namespace ct10::core {

class Bus {
 public:
  explicit Bus(std::string name);

  const std::string& name() const;
  bool driven() const;
  bool complemented() const;
  uint16_t value() const;

  void Drive(uint16_t value, bool complemented = false);
  void Clear();

 private:
  std::string name_;
  uint16_t value_ = 0;
  bool driven_ = false;
  bool complemented_ = false;
};

}  // namespace ct10::core
