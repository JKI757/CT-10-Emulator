#pragma once

#include <cstdint>

#include "core/timing_engine.h"

namespace ct10::core {

enum class MicroOp : uint8_t {
  PAR_TO_MAR,
  MEM_TO_Z,
  Z_TO_BUFFER,
  BUFFER_TO_OPCODE,
  PAR_INC,
  FORM_EFFECTIVE_ADDRESS,
  ADD_INDEX_TO_MAR,
  MAR_TO_PAR,
  ACC_TO_Y,
  BUFFER_TO_X,
  BUFFER_TO_F,
  F_TO_ACCUMULATOR,
  ACC_TO_Z,
  X_TO_Z,
  Q_TO_Z,
  BUFFER_TO_Y,
  Y_TO_MEM,
  LOAD_ACC_FROM_BUFFER,
  LOAD_X_FROM_BUFFER,
  LOAD_C_FROM_BUFFER,
  LOAD_Q_FROM_BUFFER,
  LOAD_ACC_NEGATE_BUFFER,
  STORE_ACC_TO_MEM,
  STORE_X_TO_MEM,
  STORE_Q_TO_MEM,
  COPY_MEM_TO_MEM_PLUS_ONE,
  INCREMENT_X_BY_BUFFER,
  ALU_ADD_TO_F,
  ALU_SUB_TO_F,
  ALU_AND,
  ALU_IOR,
  ALU_XOR,
  SHIFT_SLA,
  SHIFT_SRA,
  SHIFT_SLL,
  SHIFT_SRL,
  MULTIPLY,
  DIVIDE,
  RAO,
  RSO,
  BRANCH,
  SKIP_IF_INTERRUPT,
  SKIP_IF_SENSE,
  SKIP_IF_FLAG,
  FLAG_SET,
  FLAG_CLEAR,
  SENSE_STATUS,
  IO_NOOP,
  ALU_DIV,
  ALU_MUL,
  UPDATE_FLAGS,
  UPDATE_FLAGS_Q,
  UPDATE_FLAGS_AQ,
  UPDATE_OVERFLOW,
  HALT,
};

struct MicroOpStep {
  uint8_t distributor = 0;
  ClockPhase phase = ClockPhase::CP1;
  MicroOp op = MicroOp::PAR_TO_MAR;
};

}  // namespace ct10::core
