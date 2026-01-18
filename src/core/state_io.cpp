#include "core/state_io.h"

#include <cstring>
#include <fstream>
#include <vector>

namespace ct10::core {
namespace {

constexpr char kMagic[8] = {'C', 'T', '1', '0', 'D', 'M', 'P', '1'};
constexpr uint32_t kVersion = 6;

bool WriteBytes(std::ofstream& out, const void* data, size_t size) {
  out.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
  return static_cast<bool>(out);
}

bool ReadBytes(std::ifstream& in, void* data, size_t size) {
  in.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
  return static_cast<bool>(in);
}

bool WriteU8(std::ofstream& out, uint8_t value) {
  return WriteBytes(out, &value, sizeof(value));
}

bool WriteU16(std::ofstream& out, uint16_t value) {
  uint8_t bytes[2] = {static_cast<uint8_t>(value & 0xFF),
                      static_cast<uint8_t>((value >> 8) & 0xFF)};
  return WriteBytes(out, bytes, sizeof(bytes));
}

bool WriteU32(std::ofstream& out, uint32_t value) {
  uint8_t bytes[4] = {static_cast<uint8_t>(value & 0xFF),
                      static_cast<uint8_t>((value >> 8) & 0xFF),
                      static_cast<uint8_t>((value >> 16) & 0xFF),
                      static_cast<uint8_t>((value >> 24) & 0xFF)};
  return WriteBytes(out, bytes, sizeof(bytes));
}

bool ReadU8(std::ifstream& in, uint8_t& value) {
  return ReadBytes(in, &value, sizeof(value));
}

bool ReadU16(std::ifstream& in, uint16_t& value) {
  uint8_t bytes[2] = {};
  if (!ReadBytes(in, bytes, sizeof(bytes))) {
    return false;
  }
  value = static_cast<uint16_t>(bytes[0] | (bytes[1] << 8));
  return true;
}

bool ReadU32(std::ifstream& in, uint32_t& value) {
  uint8_t bytes[4] = {};
  if (!ReadBytes(in, bytes, sizeof(bytes))) {
    return false;
  }
  value = static_cast<uint32_t>(bytes[0] | (bytes[1] << 8) |
                                (bytes[2] << 16) | (bytes[3] << 24));
  return true;
}

bool WriteBool(std::ofstream& out, bool value) {
  return WriteU8(out, value ? 1 : 0);
}

bool ReadBool(std::ifstream& in, bool& value) {
  uint8_t raw = 0;
  if (!ReadU8(in, raw)) {
    return false;
  }
  value = (raw != 0);
  return true;
}

}  // namespace

bool SaveState(const MachineState& state,
               const std::string& path,
               std::string* error) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    if (error) {
      *error = "Unable to open file for writing.";
    }
    return false;
  }

  if (!WriteBytes(out, kMagic, sizeof(kMagic)) ||
      !WriteU32(out, kVersion)) {
    if (error) {
      *error = "Failed to write header.";
    }
    return false;
  }

  if (!WriteU16(out, state.accumulator.value()) ||
      !WriteU16(out, state.buffer.value()) ||
      !WriteU16(out, state.quotient.value()) ||
      !WriteU16(out, state.index.value()) ||
      !WriteU16(out, state.countdown.value()) ||
      !WriteU16(out, state.mar.value()) ||
      !WriteU16(out, state.par.value()) ||
      !WriteU16(out, state.opcode.value()) ||
      !WriteU16(out, state.distributor.value())) {
    if (error) {
      *error = "Failed to write registers.";
    }
    return false;
  }

  if (!WriteU8(out, state.timing.distributor) ||
      !WriteU8(out, static_cast<uint8_t>(state.timing.phase)) ||
      !WriteBool(out, state.timing.acquisition) ||
      !WriteBool(out, state.mode.halted)) {
    if (error) {
      *error = "Failed to write timing/mode.";
    }
    return false;
  }

  if (!WriteBool(out, state.flags.carry) ||
      !WriteBool(out, state.flags.zero) ||
      !WriteBool(out, state.flags.greater) ||
      !WriteBool(out, state.flags.less) ||
      !WriteBool(out, state.flags.add_overflow) ||
      !WriteBool(out, state.flags.divide_overflow) ||
      !WriteBool(out, state.flags.inst_error)) {
    if (error) {
      *error = "Failed to write flags.";
    }
    return false;
  }

  if (!WriteBool(out, state.status.interrupt) ||
      !WriteBool(out, state.status.sense) ||
      !WriteBool(out, state.status.flag) ||
      !WriteBool(out, state.status.wait)) {
    if (error) {
      *error = "Failed to write status flags.";
    }
    return false;
  }

  const Bus* buses[] = {&state.x_bus, &state.y_bus, &state.z_bus, &state.f_bus};
  for (const Bus* bus : buses) {
    if (!WriteU16(out, bus->value()) ||
        !WriteBool(out, bus->driven()) ||
        !WriteBool(out, bus->complemented())) {
      if (error) {
        *error = "Failed to write buses.";
      }
      return false;
    }
  }

  if (!WriteU32(out, Memory::kSize)) {
    if (error) {
      *error = "Failed to write memory header.";
    }
    return false;
  }
  for (uint16_t addr = 0; addr < Memory::kSize; ++addr) {
    if (!WriteU8(out, state.memory.Read(addr))) {
      if (error) {
        *error = "Failed to write memory.";
      }
      return false;
    }
  }

  if (!WriteU32(out, static_cast<uint32_t>(state.io.input_pos)) ||
      !WriteBool(out, state.io.interrupt) ||
      !WriteU8(out, state.io.last_command) ||
      !WriteU8(out, state.io.status) ||
      !WriteU8(out, state.io.selected_device) ||
      !WriteBool(out, state.io.hex_mode) ||
      !WriteBool(out, state.io.alpha_mode) ||
      !WriteU8(out, static_cast<uint8_t>(state.io.transfer_mode)) ||
      !WriteU16(out, state.io.transfer_address) ||
      !WriteU16(out, state.io.transfer_remaining) ||
      !WriteU8(out, state.io.wait_cycles)) {
    if (error) {
      *error = "Failed to write I/O header.";
    }
    return false;
  }

  if (!WriteU32(out, static_cast<uint32_t>(state.io.input_data.size()))) {
    if (error) {
      *error = "Failed to write input size.";
    }
    return false;
  }
  for (uint8_t value : state.io.input_data) {
    if (!WriteU8(out, value)) {
      if (error) {
        *error = "Failed to write input data.";
      }
      return false;
    }
  }

  if (!WriteU32(out, static_cast<uint32_t>(state.io.output_data.size()))) {
    if (error) {
      *error = "Failed to write output size.";
    }
    return false;
  }
  for (uint8_t value : state.io.output_data) {
    if (!WriteU8(out, value)) {
      if (error) {
        *error = "Failed to write output data.";
      }
      return false;
    }
  }

  if (!WriteU32(out, static_cast<uint32_t>(state.io.terminal_input_pos)) ||
      !WriteU32(out, static_cast<uint32_t>(state.io.terminal_input.size()))) {
    if (error) {
      *error = "Failed to write terminal input size.";
    }
    return false;
  }
  for (uint8_t value : state.io.terminal_input) {
    if (!WriteU8(out, value)) {
      if (error) {
        *error = "Failed to write terminal input data.";
      }
      return false;
    }
  }

  if (!WriteU32(out, static_cast<uint32_t>(state.io.terminal_output.size()))) {
    if (error) {
      *error = "Failed to write terminal output size.";
    }
    return false;
  }
  for (uint8_t value : state.io.terminal_output) {
    if (!WriteU8(out, value)) {
      if (error) {
        *error = "Failed to write terminal output data.";
      }
      return false;
    }
  }

  if (!WriteU32(out, static_cast<uint32_t>(state.io.printer_output.size()))) {
    if (error) {
      *error = "Failed to write printer output size.";
    }
    return false;
  }
  for (uint8_t value : state.io.printer_output) {
    if (!WriteU8(out, value)) {
      if (error) {
        *error = "Failed to write printer output data.";
      }
      return false;
    }
  }

  if (!WriteBool(out, state.panel_input.start) ||
      !WriteBool(out, state.panel_input.stop) ||
      !WriteBool(out, state.panel_input.clear) ||
      !WriteBool(out, state.panel_input.lamp_test) ||
      !WriteBool(out, state.panel_input.reset) ||
      !WriteBool(out, state.panel_input.power_on) ||
      !WriteBool(out, state.panel_input.key_pressed) ||
      !WriteBool(out, state.panel_input.has_last_key) ||
      !WriteU8(out, state.panel_input.key_value) ||
      !WriteU8(out, state.panel_input.last_key) ||
      !WriteU16(out, state.panel_input.input_switches) ||
      !WriteU8(out, state.panel_input.io_mode) ||
      !WriteU8(out, state.panel_input.mode) ||
      !WriteBool(out, state.panel_input.mem_read) ||
      !WriteBool(out, state.panel_input.mem_write) ||
      !WriteBool(out, state.panel_input.load_pressed) ||
      !WriteU8(out, static_cast<uint8_t>(state.panel_input.load_target)) ||
      !WriteBool(out, state.panel_input.rpt) ||
      !WriteBool(out, state.panel_input.sense) ||
      !WriteBool(out, state.panel_input.error_inst) ||
      !WriteBool(out, state.panel_input.error_add) ||
      !WriteBool(out, state.panel_input.error_div) ||
      !WriteBool(out, state.panel_input.io_read) ||
      !WriteBool(out, state.panel_input.io_write) ||
      !WriteBool(out, state.panel_input.io_intrp) ||
      !WriteBool(out, state.panel_input.io_block)) {
    if (error) {
      *error = "Failed to write panel input.";
    }
    return false;
  }

  return true;
}

bool LoadState(MachineState& state,
               const std::string& path,
               std::string* error) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    if (error) {
      *error = "Unable to open file for reading.";
    }
    return false;
  }

  char magic[sizeof(kMagic)] = {};
  if (!ReadBytes(in, magic, sizeof(magic)) ||
      std::memcmp(magic, kMagic, sizeof(kMagic)) != 0) {
    if (error) {
      *error = "Invalid state file header.";
    }
    return false;
  }

  uint32_t version = 0;
  if (!ReadU32(in, version) ||
      (version != 1 && version != 2 && version != 3 &&
       version != 4 && version != 5 && version != kVersion)) {
    if (error) {
      *error = "Unsupported state file version.";
    }
    return false;
  }

  uint16_t value = 0;
  if (!ReadU16(in, value)) return false;
  state.accumulator.Load(value);
  if (!ReadU16(in, value)) return false;
  state.buffer.Load(value);
  if (!ReadU16(in, value)) return false;
  state.quotient.Load(value);
  if (!ReadU16(in, value)) return false;
  state.index.Load(value);
  if (!ReadU16(in, value)) return false;
  state.countdown.Load(value);
  if (!ReadU16(in, value)) return false;
  state.mar.Load(value);
  if (!ReadU16(in, value)) return false;
  state.par.Load(value);
  if (!ReadU16(in, value)) return false;
  state.opcode.Load(value);
  if (!ReadU16(in, value)) return false;
  state.distributor.Load(value);

  uint8_t phase = 0;
  if (!ReadU8(in, state.timing.distributor) ||
      !ReadU8(in, phase) ||
      !ReadBool(in, state.timing.acquisition) ||
      !ReadBool(in, state.mode.halted)) {
    if (error) {
      *error = "Failed to read timing/mode.";
    }
    return false;
  }
  state.timing.phase = static_cast<ClockPhase>(phase);

  if (!ReadBool(in, state.flags.carry) ||
      !ReadBool(in, state.flags.zero) ||
      !ReadBool(in, state.flags.greater) ||
      !ReadBool(in, state.flags.less) ||
      !ReadBool(in, state.flags.add_overflow) ||
      !ReadBool(in, state.flags.divide_overflow)) {
    if (error) {
      *error = "Failed to read flags.";
    }
    return false;
  }
  if (version >= 2) {
    if (!ReadBool(in, state.flags.inst_error)) {
      if (error) {
        *error = "Failed to read instruction error flag.";
      }
      return false;
    }
  } else {
    state.flags.inst_error = false;
  }

  if (!ReadBool(in, state.status.interrupt) ||
      !ReadBool(in, state.status.sense) ||
      !ReadBool(in, state.status.flag) ||
      !ReadBool(in, state.status.wait)) {
    if (error) {
      *error = "Failed to read status flags.";
    }
    return false;
  }

  Bus* buses[] = {&state.x_bus, &state.y_bus, &state.z_bus, &state.f_bus};
  for (Bus* bus : buses) {
    uint16_t bus_value = 0;
    bool driven = false;
    bool complemented = false;
    if (!ReadU16(in, bus_value) ||
        !ReadBool(in, driven) ||
        !ReadBool(in, complemented)) {
      if (error) {
        *error = "Failed to read buses.";
      }
      return false;
    }
    if (driven) {
      bus->Drive(bus_value, complemented);
    } else {
      bus->Clear();
    }
  }

  uint32_t mem_size = 0;
  if (!ReadU32(in, mem_size) || mem_size != Memory::kSize) {
    if (error) {
      *error = "Unexpected memory size.";
    }
    return false;
  }
  for (uint16_t addr = 0; addr < Memory::kSize; ++addr) {
    uint8_t byte = 0;
    if (!ReadU8(in, byte)) {
      if (error) {
        *error = "Failed to read memory.";
      }
      return false;
    }
    state.memory.Write(addr, byte);
  }

  uint32_t input_pos = 0;
  if (!ReadU32(in, input_pos) ||
      !ReadBool(in, state.io.interrupt) ||
      !ReadU8(in, state.io.last_command) ||
      !ReadU8(in, state.io.status) ||
      !ReadU8(in, state.io.selected_device) ||
      !ReadBool(in, state.io.hex_mode) ||
      !ReadBool(in, state.io.alpha_mode)) {
    if (error) {
      *error = "Failed to read I/O header.";
    }
    return false;
  }
  state.io.input_pos = static_cast<size_t>(input_pos);

  if (version >= 5) {
    uint8_t mode = 0;
    if (!ReadU8(in, mode) ||
        !ReadU16(in, state.io.transfer_address) ||
        !ReadU16(in, state.io.transfer_remaining) ||
        !ReadU8(in, state.io.wait_cycles)) {
      if (error) {
        *error = "Failed to read I/O transfer state.";
      }
      return false;
    }
    state.io.transfer_mode = static_cast<IoTransferMode>(mode);
  } else if (version == 4) {
    uint8_t mode = 0;
    if (!ReadU8(in, mode) ||
        !ReadU16(in, state.io.transfer_address) ||
        !ReadU16(in, state.io.transfer_remaining)) {
      if (error) {
        *error = "Failed to read I/O transfer state.";
      }
      return false;
    }
    state.io.transfer_mode = static_cast<IoTransferMode>(mode);
    state.io.wait_cycles = 0;
  } else if (version == 3) {
    bool manual_wait = false;
    bool manual_output = false;
    uint16_t manual_address = 0;
    uint16_t manual_remaining = 0;
    if (!ReadBool(in, manual_wait) ||
        !ReadBool(in, manual_output) ||
        !ReadU16(in, manual_address) ||
        !ReadU16(in, manual_remaining)) {
      if (error) {
        *error = "Failed to read manual I/O state.";
      }
      return false;
    }
    state.io.transfer_mode = IoTransferMode::None;
    state.io.transfer_address = 0;
    state.io.transfer_remaining = 0;
    state.io.wait_cycles = 0;
  } else {
    state.io.transfer_mode = IoTransferMode::None;
    state.io.transfer_address = 0;
    state.io.transfer_remaining = 0;
    state.io.wait_cycles = 0;
  }

  uint32_t input_size = 0;
  if (!ReadU32(in, input_size)) {
    if (error) {
      *error = "Failed to read input size.";
    }
    return false;
  }
  state.io.input_data.resize(input_size);
  for (uint32_t i = 0; i < input_size; ++i) {
    uint8_t byte = 0;
    if (!ReadU8(in, byte)) {
      if (error) {
        *error = "Failed to read input data.";
      }
      return false;
    }
    state.io.input_data[i] = byte;
  }

  uint32_t output_size = 0;
  if (!ReadU32(in, output_size)) {
    if (error) {
      *error = "Failed to read output size.";
    }
    return false;
  }
  state.io.output_data.resize(output_size);
  for (uint32_t i = 0; i < output_size; ++i) {
    uint8_t byte = 0;
    if (!ReadU8(in, byte)) {
      if (error) {
        *error = "Failed to read output data.";
      }
      return false;
    }
    state.io.output_data[i] = byte;
  }

  if (version >= 6) {
    uint32_t terminal_pos = 0;
    uint32_t terminal_input_size = 0;
    if (!ReadU32(in, terminal_pos) ||
        !ReadU32(in, terminal_input_size)) {
      if (error) {
        *error = "Failed to read terminal input size.";
      }
      return false;
    }
    state.io.terminal_input_pos = static_cast<size_t>(terminal_pos);
    state.io.terminal_input.resize(terminal_input_size);
    for (uint32_t i = 0; i < terminal_input_size; ++i) {
      uint8_t byte = 0;
      if (!ReadU8(in, byte)) {
        if (error) {
          *error = "Failed to read terminal input data.";
        }
        return false;
      }
      state.io.terminal_input[i] = byte;
    }
    if (state.io.terminal_input_pos > state.io.terminal_input.size()) {
      state.io.terminal_input_pos = state.io.terminal_input.size();
    }

    uint32_t terminal_output_size = 0;
    if (!ReadU32(in, terminal_output_size)) {
      if (error) {
        *error = "Failed to read terminal output size.";
      }
      return false;
    }
    state.io.terminal_output.resize(terminal_output_size);
    for (uint32_t i = 0; i < terminal_output_size; ++i) {
      uint8_t byte = 0;
      if (!ReadU8(in, byte)) {
        if (error) {
          *error = "Failed to read terminal output data.";
        }
        return false;
      }
      state.io.terminal_output[i] = byte;
    }

    uint32_t printer_output_size = 0;
    if (!ReadU32(in, printer_output_size)) {
      if (error) {
        *error = "Failed to read printer output size.";
      }
      return false;
    }
    state.io.printer_output.resize(printer_output_size);
    for (uint32_t i = 0; i < printer_output_size; ++i) {
      uint8_t byte = 0;
      if (!ReadU8(in, byte)) {
        if (error) {
          *error = "Failed to read printer output data.";
        }
        return false;
      }
      state.io.printer_output[i] = byte;
    }
  } else {
    state.io.terminal_input.clear();
    state.io.terminal_output.clear();
    state.io.printer_output.clear();
    state.io.terminal_input_pos = 0;
  }

  if (!ReadBool(in, state.panel_input.start) ||
      !ReadBool(in, state.panel_input.stop) ||
      !ReadBool(in, state.panel_input.clear) ||
      !ReadBool(in, state.panel_input.lamp_test) ||
      !ReadBool(in, state.panel_input.reset) ||
      !ReadBool(in, state.panel_input.power_on) ||
      !ReadBool(in, state.panel_input.key_pressed) ||
      !ReadBool(in, state.panel_input.has_last_key) ||
      !ReadU8(in, state.panel_input.key_value) ||
      !ReadU8(in, state.panel_input.last_key) ||
      !ReadU16(in, state.panel_input.input_switches) ||
      !ReadU8(in, state.panel_input.io_mode) ||
      !ReadU8(in, state.panel_input.mode) ||
      !ReadBool(in, state.panel_input.mem_read) ||
      !ReadBool(in, state.panel_input.mem_write) ||
      !ReadBool(in, state.panel_input.load_pressed)) {
    if (error) {
      *error = "Failed to read panel input.";
    }
    return false;
  }
  uint8_t load_target = 0;
  if (!ReadU8(in, load_target)) {
    if (error) {
      *error = "Failed to read panel input.";
    }
    return false;
  }
  state.panel_input.load_target = static_cast<LoadTarget>(load_target);

  if (!ReadBool(in, state.panel_input.rpt) ||
      !ReadBool(in, state.panel_input.sense) ||
      !ReadBool(in, state.panel_input.error_inst) ||
      !ReadBool(in, state.panel_input.error_add) ||
      !ReadBool(in, state.panel_input.error_div) ||
      !ReadBool(in, state.panel_input.io_read) ||
      !ReadBool(in, state.panel_input.io_write) ||
      !ReadBool(in, state.panel_input.io_intrp) ||
      !ReadBool(in, state.panel_input.io_block)) {
    if (error) {
      *error = "Failed to read panel input.";
    }
    return false;
  }

  return true;
}

}  // namespace ct10::core
