#include "core/execution_engine.h"

#include "core/microcode_table.h"

namespace ct10::core {
namespace {

uint8_t ToByte(uint16_t value) {
  return static_cast<uint8_t>(value & 0xFF);
}

uint16_t FormPageAddress(uint8_t opcode, uint8_t low) {
  uint16_t page = static_cast<uint16_t>(opcode & 0x03);
  return static_cast<uint16_t>((page << 8) | low);
}

uint8_t PageBitsFromAddress(uint16_t address) {
  return static_cast<uint8_t>((address >> 8) & 0x03);
}

uint8_t EncodeBunOpcode(uint16_t address) {
  return static_cast<uint8_t>(0x90 | PageBitsFromAddress(address));
}

bool IsIndexed(uint8_t opcode) {
  return (opcode & 0x04u) != 0;
}

uint8_t BuildStatusByte(const MachineState& state) {
  uint8_t status = 0;
  if (state.status.interrupt) {
    status |= 0x01;
  }
  if (state.status.sense) {
    status |= 0x02;
  }
  if (state.status.flag) {
    status |= 0x04;
  }
  return status;
}

bool IsManualTransfer(IoTransferMode mode) {
  return mode == IoTransferMode::ManualOutput ||
         mode == IoTransferMode::ManualInput;
}

bool IsReadTransfer(IoTransferMode mode) {
  return mode == IoTransferMode::ReadBlock ||
         mode == IoTransferMode::ReadInterrupt;
}

bool IsWriteTransfer(IoTransferMode mode) {
  return mode == IoTransferMode::WriteBlock;
}

std::vector<uint8_t>& OutputBufferForDevice(MachineState& state) {
  switch (state.io.selected_device) {
    case 1:
      return state.io.terminal_output;
    case 2:
      return state.io.printer_output;
    case 0:
    default:
      return state.io.output_data;
  }
}

void UpdateTransferCountdown(MachineState& state) {
  uint16_t remaining = state.io.transfer_remaining;
  uint16_t count = (remaining > 0) ? static_cast<uint16_t>(remaining - 1) : 0;
  state.countdown.Load(static_cast<uint8_t>(count & 0xFF));
}

bool ReadInputByte(MachineState& state, uint8_t& value) {
  switch (state.io.selected_device) {
    case 1:
      if (state.io.terminal_input_pos < state.io.terminal_input.size()) {
        value = state.io.terminal_input[state.io.terminal_input_pos++];
        return true;
      }
      break;
    case 2:
      break;
    case 0:
    default:
      if (state.io.input_pos < state.io.input_data.size()) {
        value = state.io.input_data[state.io.input_pos++];
        return true;
      }
      break;
  }
  state.io.interrupt = true;
  value = 0;
  return false;
}

void TransferStep(MachineState& state) {
  IoTransferMode mode = state.io.transfer_mode;
  if (mode == IoTransferMode::None) {
    return;
  }

  if (mode != IoTransferMode::ReadInterrupt &&
      state.io.transfer_remaining == 0) {
    state.io.transfer_mode = IoTransferMode::None;
    state.io.wait_cycles = 0;
    return;
  }

  if (mode == IoTransferMode::ReadInterrupt && state.io.interrupt) {
    state.io.transfer_mode = IoTransferMode::None;
    state.io.wait_cycles = 0;
    return;
  }

  if (IsWriteTransfer(mode)) {
    uint8_t value = state.memory.Read(state.io.transfer_address);
    state.buffer.Load(value);
    OutputBufferForDevice(state).push_back(value);
  } else if (IsReadTransfer(mode)) {
    uint8_t value = 0;
    bool has_value = ReadInputByte(state, value);
    if (!has_value && mode == IoTransferMode::ReadInterrupt) {
      state.io.transfer_mode = IoTransferMode::None;
      return;
    }
    state.buffer.Load(value);
    state.memory.Write(state.io.transfer_address, value);
  } else if (mode == IoTransferMode::ManualOutput) {
    uint8_t value = state.memory.Read(state.io.transfer_address);
    state.buffer.Load(value);
    uint16_t upper = static_cast<uint16_t>(state.panel_input.input_switches & 0x300);
    state.panel_input.input_switches = static_cast<uint16_t>(upper | value);
  } else if (mode == IoTransferMode::ManualInput) {
    uint8_t value = static_cast<uint8_t>(state.panel_input.input_switches & 0xFF);
    state.buffer.Load(value);
    state.memory.Write(state.io.transfer_address, value);
  }

  state.io.transfer_address =
      static_cast<uint16_t>(state.io.transfer_address + 1);
  state.mar.Load(state.io.transfer_address);

  if (mode != IoTransferMode::ReadInterrupt) {
    state.io.transfer_remaining =
        static_cast<uint16_t>(state.io.transfer_remaining - 1);
    UpdateTransferCountdown(state);
  }

  if (mode != IoTransferMode::ReadInterrupt &&
      state.io.transfer_remaining == 0) {
    state.io.transfer_mode = IoTransferMode::None;
    state.io.wait_cycles = 0;
  }
}

void BeginTransfer(MachineState& state, IoTransferMode mode) {
  state.io.transfer_mode = mode;
  state.io.transfer_address = state.mar.value();
  state.io.wait_cycles = 0;
  if (mode == IoTransferMode::ReadBlock ||
      mode == IoTransferMode::ReadInterrupt) {
    OutputBufferForDevice(state).push_back(0x11);
  }
  if (mode == IoTransferMode::ReadInterrupt) {
    state.io.transfer_remaining = 0;
  } else {
    state.io.transfer_remaining =
        static_cast<uint16_t>(state.countdown.value()) + 1;
    UpdateTransferCountdown(state);
  }
  TransferStep(state);
}

void HandleIo(MachineState& state) {
  if (state.panel_input.io_mode == 1) {
    state.io.status = BuildStatusByte(state);
    return;
  }
  uint8_t op = static_cast<uint8_t>(state.opcode.value() & 0xF8);
  switch (op) {
    case 0xD0:
      if (state.io.transfer_mode == IoTransferMode::None) {
        BeginTransfer(state, IoTransferMode::WriteBlock);
      }
      break;
    case 0xD8:
      if (state.io.transfer_mode == IoTransferMode::None) {
        BeginTransfer(state, IoTransferMode::ManualOutput);
      }
      break;
    case 0xE0:
      if (state.io.transfer_mode == IoTransferMode::None) {
        BeginTransfer(state, IoTransferMode::ReadBlock);
      }
      break;
    case 0xE8:
      if (state.io.transfer_mode == IoTransferMode::None) {
        BeginTransfer(state, IoTransferMode::ReadInterrupt);
      }
      break;
    case 0xF0:
      if (state.io.transfer_mode == IoTransferMode::None) {
        BeginTransfer(state, IoTransferMode::ManualInput);
      }
      break;
    default:
      break;
  }
  state.io.status = BuildStatusByte(state);
}

}  // namespace

void ExecutionEngine::Step(MachineState& state) const {
  if (state.mode.halted) {
    return;
  }

  if (state.io.transfer_mode != IoTransferMode::None) {
    state.status.wait = true;
    if (IsManualTransfer(state.io.transfer_mode)) {
      state.mode.halted = true;
      if (!state.panel_input.start) {
        return;
      }
      TransferStep(state);
      return;
    }
    if (state.io.wait_cycles > 0) {
      --state.io.wait_cycles;
      return;
    }
    TransferStep(state);
    if (state.io.transfer_mode != IoTransferMode::None) {
      state.io.wait_cycles = 1;
    }
    return;
  }

  state.status.wait = false;

  if (!state.timing.acquisition &&
      state.timing.distributor == 0 &&
      state.timing.phase == ClockPhase::CP1) {
    state.flags.add_overflow = false;
    state.flags.divide_overflow = false;
    state.flags.inst_error = false;
  }

  switch (state.panel_input.io_mode) {
    case 2:
      state.io.hex_mode = true;
      state.io.alpha_mode = false;
      break;
    case 3:
      state.io.hex_mode = false;
      state.io.alpha_mode = true;
      break;
    case 1:
      state.io.hex_mode = false;
      state.io.alpha_mode = false;
      break;
    case 0:
    default:
      break;
  }

  state.status.sense = state.panel_input.sense;
  state.status.interrupt = state.io.interrupt;
  state.io.status = BuildStatusByte(state);

  if (state.timing.phase == ClockPhase::CP1) {
    state.x_bus.Clear();
    state.y_bus.Clear();
    state.z_bus.Clear();
  } else if (state.timing.phase == ClockPhase::CP2) {
    state.f_bus.Clear();
  }

  const auto& steps = state.timing.acquisition
                          ? MicrocodeTable::Acquisition()
                          : MicrocodeTable::Execution(ToByte(state.opcode.value()));
  if (!state.timing.acquisition && steps.empty()) {
    state.flags.inst_error = true;
    if (!state.panel_input.error_inst) {
      state.mode.halted = true;
      return;
    }
  }

  for (const auto& step : steps) {
    if (step.distributor == state.timing.distributor &&
        step.phase == state.timing.phase) {
      ExecuteMicroOp(step.op, state);
      state.AddTrace(step.op);
    }
  }

  state.distributor.Load(state.timing.distributor);
}

void ExecutionEngine::ExecuteMicroOp(MicroOp op, MachineState& state) const {
  switch (op) {
    case MicroOp::PAR_TO_MAR: {
      uint16_t value = state.par.value();
      state.z_bus.Drive(value);
      state.mar.Load(value);
      break;
    }
    case MicroOp::MEM_TO_Z: {
      uint8_t value = state.memory.Read(state.mar.value());
      state.z_bus.Drive(static_cast<uint16_t>(~value & 0xFF), true);
      break;
    }
    case MicroOp::Z_TO_BUFFER: {
      uint8_t value = ToByte(state.z_bus.value());
      if (state.z_bus.complemented()) {
        value = static_cast<uint8_t>(~value);
      }
      state.buffer.Load(value);
      break;
    }
    case MicroOp::BUFFER_TO_OPCODE:
      state.opcode.Load(state.buffer.value());
      break;
    case MicroOp::PAR_INC:
      if (!(state.panel_input.rpt &&
            (state.panel_input.mode == 1 || state.panel_input.mode == 2))) {
        state.par.Increment();
      }
      break;
    case MicroOp::FORM_EFFECTIVE_ADDRESS: {
      uint16_t addr = FormPageAddress(ToByte(state.opcode.value()),
                                      ToByte(state.buffer.value()));
      state.mar.Load(addr);
      break;
    }
    case MicroOp::ADD_INDEX_TO_MAR:
      if (IsIndexed(ToByte(state.opcode.value()))) {
        state.mar.Load(state.mar.value() + state.index.value());
      }
      break;
    case MicroOp::MAR_TO_PAR:
      state.par.Load(state.mar.value());
      break;
    case MicroOp::ACC_TO_Y:
      state.y_bus.Drive(state.accumulator.value());
      break;
    case MicroOp::BUFFER_TO_X:
      state.x_bus.Drive(state.buffer.value());
      break;
    case MicroOp::BUFFER_TO_F:
      state.f_bus.Drive(state.buffer.value());
      break;
    case MicroOp::F_TO_ACCUMULATOR:
      state.accumulator.Load(state.f_bus.value());
      break;
    case MicroOp::ACC_TO_Z: {
      uint8_t value = ToByte(state.accumulator.value());
      state.z_bus.Drive(static_cast<uint16_t>(~value & 0xFF), true);
      break;
    }
    case MicroOp::X_TO_Z: {
      uint8_t value = ToByte(state.index.value());
      state.z_bus.Drive(static_cast<uint16_t>(~value & 0xFF), true);
      break;
    }
    case MicroOp::Q_TO_Z: {
      uint8_t value = ToByte(state.quotient.value());
      state.z_bus.Drive(static_cast<uint16_t>(~value & 0xFF), true);
      break;
    }
    case MicroOp::BUFFER_TO_Y:
      state.y_bus.Drive(state.buffer.value());
      break;
    case MicroOp::Y_TO_MEM:
      state.memory.Write(state.mar.value(), ToByte(state.y_bus.value()));
      break;
    case MicroOp::LOAD_ACC_FROM_BUFFER:
      state.accumulator.Load(state.buffer.value());
      break;
    case MicroOp::LOAD_X_FROM_BUFFER:
      state.index.Load(state.buffer.value());
      break;
    case MicroOp::LOAD_C_FROM_BUFFER:
      state.countdown.Load(state.buffer.value());
      break;
    case MicroOp::LOAD_Q_FROM_BUFFER:
      state.quotient.Load(state.buffer.value());
      break;
    case MicroOp::LOAD_ACC_NEGATE_BUFFER: {
      uint8_t value = ToByte(state.buffer.value());
      uint8_t result = static_cast<uint8_t>(~value + 1);
      state.accumulator.Load(result);
      break;
    }
    case MicroOp::STORE_ACC_TO_MEM:
      state.memory.Write(state.mar.value(), ToByte(state.accumulator.value()));
      break;
    case MicroOp::STORE_X_TO_MEM:
      state.memory.Write(state.mar.value(), ToByte(state.index.value()));
      break;
    case MicroOp::STORE_Q_TO_MEM:
      state.memory.Write(state.mar.value(), ToByte(state.quotient.value()));
      break;
    case MicroOp::COPY_MEM_TO_MEM_PLUS_ONE: {
      uint16_t addr = state.mar.value();
      uint8_t value = state.memory.Read(addr);
      uint16_t next = static_cast<uint16_t>(addr + 1);
      state.memory.Write(next, value);
      state.mar.Load(next);
      break;
    }
    case MicroOp::INCREMENT_X_BY_BUFFER:
      state.index.Load(state.index.value() + state.buffer.value());
      break;
    case MicroOp::ALU_ADD_TO_F: {
      uint8_t a = ToByte(state.y_bus.value());
      uint8_t b = ToByte(state.x_bus.value());
      uint16_t sum = static_cast<uint16_t>(a) + static_cast<uint16_t>(b);
      uint8_t result = static_cast<uint8_t>(sum & 0xFF);
      state.f_bus.Drive(result);
      state.flags.carry = sum > 0xFF;
      state.flags.add_overflow =
          ((a ^ result) & (b ^ result) & 0x80) != 0;
      if (state.flags.add_overflow && !state.panel_input.error_add) {
        state.mode.halted = true;
      }
      break;
    }
    case MicroOp::ALU_SUB_TO_F: {
      uint8_t a = ToByte(state.y_bus.value());
      uint8_t b = ToByte(state.x_bus.value());
      int16_t diff = static_cast<int16_t>(a) - static_cast<int16_t>(b);
      uint8_t result = static_cast<uint8_t>(diff & 0xFF);
      state.f_bus.Drive(result);
      state.flags.carry = diff >= 0;
      state.flags.add_overflow =
          ((a ^ b) & (a ^ result) & 0x80) != 0;
      if (state.flags.add_overflow && !state.panel_input.error_add) {
        state.mode.halted = true;
      }
      break;
    }
    case MicroOp::ALU_AND: {
      uint8_t result = static_cast<uint8_t>(ToByte(state.accumulator.value()) &
                                            ToByte(state.buffer.value()));
      state.accumulator.Load(result);
      break;
    }
    case MicroOp::ALU_IOR: {
      uint8_t result = static_cast<uint8_t>(ToByte(state.accumulator.value()) |
                                            ToByte(state.buffer.value()));
      state.accumulator.Load(result);
      break;
    }
    case MicroOp::ALU_XOR: {
      uint8_t result = static_cast<uint8_t>(ToByte(state.accumulator.value()) ^
                                            ToByte(state.buffer.value()));
      state.accumulator.Load(result);
      break;
    }
    case MicroOp::SHIFT_SLA: {
      uint16_t value = static_cast<uint16_t>((state.accumulator.value() << 8) |
                                             ToByte(state.quotient.value()));
      uint8_t count = ToByte(state.buffer.value());
      if (count >= 16) {
        value = 0;
      } else {
        value = static_cast<uint16_t>(value << count);
      }
      state.accumulator.Load(static_cast<uint8_t>((value >> 8) & 0xFF));
      state.quotient.Load(static_cast<uint8_t>(value & 0xFF));
      break;
    }
    case MicroOp::SHIFT_SRA: {
      int16_t value = static_cast<int16_t>((state.accumulator.value() << 8) |
                                           ToByte(state.quotient.value()));
      uint8_t count = ToByte(state.buffer.value());
      if (count >= 16) {
        value = (value < 0) ? static_cast<int16_t>(-1) : 0;
      } else {
        value = static_cast<int16_t>(value >> count);
      }
      state.accumulator.Load(static_cast<uint8_t>((value >> 8) & 0xFF));
      state.quotient.Load(static_cast<uint8_t>(value & 0xFF));
      break;
    }
    case MicroOp::SHIFT_SLL: {
      uint8_t value = ToByte(state.accumulator.value());
      uint8_t count = ToByte(state.buffer.value());
      if (count >= 8) {
        value = 0;
      } else {
        value = static_cast<uint8_t>(value << count);
      }
      state.accumulator.Load(value);
      break;
    }
    case MicroOp::SHIFT_SRL: {
      uint8_t value = ToByte(state.accumulator.value());
      uint8_t count = ToByte(state.buffer.value());
      if (count >= 8) {
        value = 0;
      } else {
        value = static_cast<uint8_t>(value >> count);
      }
      state.accumulator.Load(value);
      break;
    }
    case MicroOp::MULTIPLY: {
      int16_t a = static_cast<int8_t>(ToByte(state.accumulator.value()));
      int16_t b = static_cast<int8_t>(ToByte(state.buffer.value()));
      int16_t product = static_cast<int16_t>(a * b);
      state.accumulator.Load(static_cast<uint8_t>((product >> 8) & 0xFF));
      state.quotient.Load(static_cast<uint8_t>(product & 0xFF));
      break;
    }
    case MicroOp::DIVIDE: {
      int16_t dividend = static_cast<int16_t>((state.accumulator.value() << 8) |
                                              ToByte(state.quotient.value()));
      int16_t divisor = static_cast<int8_t>(ToByte(state.buffer.value()));
      if (divisor == 0) {
        state.flags.divide_overflow = true;
        if (!state.panel_input.error_div) {
          state.mode.halted = true;
        }
        break;
      }
      int16_t quotient = static_cast<int16_t>(dividend / divisor);
      int16_t remainder = static_cast<int16_t>(dividend % divisor);
      if (quotient < -128 || quotient > 127) {
        state.flags.divide_overflow = true;
        if (!state.panel_input.error_div) {
          state.mode.halted = true;
        }
        break;
      }
      state.flags.divide_overflow = false;
      state.quotient.Load(static_cast<uint8_t>(quotient & 0xFF));
      state.accumulator.Load(static_cast<uint8_t>(remainder & 0xFF));
      break;
    }
    case MicroOp::RAO: {
      uint8_t value = ToByte(state.buffer.value());
      uint16_t sum = static_cast<uint16_t>(value) + 1u;
      uint8_t result = static_cast<uint8_t>(sum & 0xFF);
      state.memory.Write(state.mar.value(), result);
      state.accumulator.Load(result);
      state.flags.carry = value == 0xFF;
      break;
    }
    case MicroOp::RSO: {
      uint8_t value = ToByte(state.buffer.value());
      int16_t diff = static_cast<int16_t>(value) - 1;
      uint8_t result = static_cast<uint8_t>(diff & 0xFF);
      state.memory.Write(state.mar.value(), result);
      state.accumulator.Load(result);
      state.flags.carry = value == 0x00;
      break;
    }
    case MicroOp::BRANCH: {
      uint8_t op = ToByte(state.opcode.value()) & 0xF8;
      bool take = false;
      switch (op) {
        case 0x90:
          take = true;
          break;
        case 0x98:
          take = true;
          state.mode.halted = true;
          break;
        case 0xA0:
          take = true;
          break;
        case 0xA8:
          take = state.flags.greater;
          break;
        case 0xB0:
          take = state.flags.zero;
          break;
        case 0xB8:
          take = state.flags.less;
          break;
        case 0xC0:
          take = !state.flags.carry;
          break;
        case 0xC8:
          take = state.index.value() == 0;
          break;
        default:
          break;
      }
      if (op == 0xA0) {
        uint16_t addr = state.mar.value();
        state.memory.Write(addr, EncodeBunOpcode(state.par.value()));
        state.memory.Write(static_cast<uint16_t>(addr + 1),
                           static_cast<uint8_t>(state.par.value() & 0xFF));
        state.par.Load(static_cast<uint16_t>(addr + 2));
      } else if (take) {
        state.par.Load(state.mar.value());
      }
      break;
    }
    case MicroOp::SKIP_IF_INTERRUPT: {
      state.countdown.Load(state.buffer.value());
      if (state.status.interrupt) {
        state.par.Load(state.par.value() +
                       static_cast<uint16_t>(2u * ToByte(state.buffer.value())));
      }
      state.io.interrupt = false;
      break;
    }
    case MicroOp::SKIP_IF_SENSE: {
      state.countdown.Load(state.buffer.value());
      if (state.status.sense) {
        state.par.Load(state.par.value() +
                       static_cast<uint16_t>(2u * ToByte(state.buffer.value())));
      }
      break;
    }
    case MicroOp::SKIP_IF_FLAG: {
      state.countdown.Load(state.buffer.value());
      if (state.status.flag) {
        state.par.Load(state.par.value() +
                       static_cast<uint16_t>(2u * ToByte(state.buffer.value())));
      }
      break;
    }
    case MicroOp::FLAG_SET:
      state.status.flag = true;
      break;
    case MicroOp::FLAG_CLEAR:
      state.status.flag = false;
      break;
    case MicroOp::SENSE_STATUS:
      state.accumulator.Load(state.io.status);
      break;
    case MicroOp::IO_NOOP:
      if (ToByte(state.opcode.value()) == 0x11) {
        uint8_t cmd = ToByte(state.buffer.value());
        state.io.last_command = cmd;
        state.io.selected_device = static_cast<uint8_t>(cmd & 0x07);
        state.io.hex_mode = (cmd & 0x08) != 0;
        state.io.alpha_mode = (cmd & 0x10) != 0;
      }
      HandleIo(state);
      break;
    case MicroOp::ALU_DIV:
    case MicroOp::ALU_MUL:
      break;
    case MicroOp::UPDATE_FLAGS: {
      uint8_t value = ToByte(state.accumulator.value());
      state.flags.zero = value == 0;
      state.flags.greater = (value & 0x80) == 0 && value != 0;
      state.flags.less = (value & 0x80) != 0;
      break;
    }
    case MicroOp::UPDATE_FLAGS_Q: {
      uint8_t value = ToByte(state.quotient.value());
      state.flags.zero = value == 0;
      state.flags.greater = (value & 0x80) == 0 && value != 0;
      state.flags.less = (value & 0x80) != 0;
      break;
    }
    case MicroOp::UPDATE_FLAGS_AQ: {
      uint16_t value = static_cast<uint16_t>((state.accumulator.value() << 8) |
                                             ToByte(state.quotient.value()));
      state.flags.zero = value == 0;
      state.flags.greater = (value & 0x8000) == 0 && value != 0;
      state.flags.less = (value & 0x8000) != 0;
      break;
    }
    case MicroOp::UPDATE_OVERFLOW:
      break;
    case MicroOp::HALT:
      state.mode.halted = true;
      break;
  }
}

}  // namespace ct10::core
