#pragma once

#include <vector>

#include "core/bus.h"
#include "core/memory.h"
#include "core/microcode.h"
#include "core/panel_input.h"
#include "core/register.h"
#include "core/timing_engine.h"

namespace ct10::core {

struct Flags {
  bool carry = false;
  bool zero = false;
  bool greater = false;
  bool less = false;
  bool add_overflow = false;
  bool divide_overflow = false;
  bool inst_error = false;
};

struct ModeState {
  bool halted = false;
};

struct StatusFlags {
  bool interrupt = false;
  bool sense = false;
  bool flag = false;
  bool wait = false;
};

enum class IoTransferMode : uint8_t {
  None = 0,
  ManualOutput,
  ManualInput,
  ReadBlock,
  WriteBlock,
  ReadInterrupt,
};

struct IOState {
  std::vector<uint8_t> input_data;
  std::vector<uint8_t> output_data;
  size_t input_pos = 0;
  std::vector<uint8_t> terminal_input;
  std::vector<uint8_t> terminal_output;
  size_t terminal_input_pos = 0;
  std::vector<uint8_t> printer_output;
  bool interrupt = false;
  uint8_t last_command = 0;
  uint8_t status = 0;
  uint8_t selected_device = 0;
  bool hex_mode = false;
  bool alpha_mode = false;
  IoTransferMode transfer_mode = IoTransferMode::None;
  uint16_t transfer_address = 0;
  uint16_t transfer_remaining = 0;
  uint8_t wait_cycles = 0;
};

struct TraceEntry {
  uint8_t distributor = 0;
  ClockPhase phase = ClockPhase::CP1;
  bool acquisition = true;
  MicroOp op = MicroOp::PAR_TO_MAR;
};

class MachineState {
 public:
  MachineState();

  void Reset();
  void ClearRegisters();
  void AddTrace(MicroOp op);
  void ClearTrace();

  Register accumulator;
  Register buffer;
  Register quotient;
  Register index;
  Register countdown;
  Register mar;
  Register par;
  Register opcode;
  Register distributor;

  Bus x_bus;
  Bus y_bus;
  Bus z_bus;
  Bus f_bus;

  Memory memory;
  Flags flags;
  TimingState timing;
  ModeState mode;
  StatusFlags status;
  IOState io;
  PanelInput panel_input;
  std::vector<TraceEntry> trace;

  static constexpr size_t kTraceCapacity = 512;
};

}  // namespace ct10::core
