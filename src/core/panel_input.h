#pragma once

#include <cstdint>

namespace ct10::core {

enum class LoadTarget : uint8_t {
  None = 0,
  Accumulator,
  Buffer,
  Countdown,
  Distributor,
  Opcode,
  Mar,
  Par,
  Quotient,
  Index,
};

struct PanelInput {
  bool start = false;
  bool stop = false;
  bool clear = false;
  bool lamp_test = false;
  bool reset = false;
  bool power_on = true;
  bool key_pressed = false;
  bool has_last_key = false;
  uint8_t key_value = 0;
  uint8_t last_key = 0;
  uint16_t input_switches = 0;
  uint8_t io_mode = 1;
  uint8_t mode = 0;
  bool mem_read = false;
  bool mem_write = false;
  bool load_pressed = false;
  LoadTarget load_target = LoadTarget::None;
  bool rpt = false;
  bool sense = false;
  bool error_inst = false;
  bool error_add = false;
  bool error_div = false;
  bool io_read = false;
  bool io_write = false;
  bool io_intrp = false;
  bool io_block = false;

  void ClearMomentary() {
    start = false;
    stop = false;
    clear = false;
    lamp_test = false;
    reset = false;
    key_pressed = false;
    load_pressed = false;
    load_target = LoadTarget::None;
  }
};

}  // namespace ct10::core
