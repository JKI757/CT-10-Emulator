#include "core/microcode_table.h"

#include <array>

namespace ct10::core {
namespace {

std::vector<MicroOpStep> BuildAcquisition() {
  return {
      {0, ClockPhase::CP1, MicroOp::PAR_TO_MAR},
      {0, ClockPhase::CP2, MicroOp::MEM_TO_Z},
      {0, ClockPhase::CP3, MicroOp::Z_TO_BUFFER},
      {1, ClockPhase::CP1, MicroOp::BUFFER_TO_OPCODE},
      {1, ClockPhase::CP2, MicroOp::PAR_INC},
  };
}

std::vector<MicroOpStep> BuildImmediateFetch() {
  return {
      {2, ClockPhase::CP1, MicroOp::PAR_TO_MAR},
      {2, ClockPhase::CP2, MicroOp::MEM_TO_Z},
      {2, ClockPhase::CP3, MicroOp::Z_TO_BUFFER},
      {3, ClockPhase::CP1, MicroOp::PAR_INC},
  };
}

std::vector<MicroOpStep> BuildAddressOnlySequence() {
  return {
      {2, ClockPhase::CP1, MicroOp::PAR_TO_MAR},
      {2, ClockPhase::CP2, MicroOp::MEM_TO_Z},
      {2, ClockPhase::CP3, MicroOp::Z_TO_BUFFER},
      {3, ClockPhase::CP1, MicroOp::FORM_EFFECTIVE_ADDRESS},
      {3, ClockPhase::CP2, MicroOp::ADD_INDEX_TO_MAR},
      {3, ClockPhase::CP3, MicroOp::PAR_INC},
  };
}

std::vector<MicroOpStep> BuildMemoryOperandSequence() {
  auto ops = BuildAddressOnlySequence();
  ops.push_back({4, ClockPhase::CP2, MicroOp::MEM_TO_Z});
  ops.push_back({4, ClockPhase::CP3, MicroOp::Z_TO_BUFFER});
  return ops;
}

std::vector<MicroOpStep> BuildLda() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::LOAD_ACC_FROM_BUFFER});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildLai() {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, MicroOp::LOAD_ACC_FROM_BUFFER});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildLxi() {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, MicroOp::LOAD_X_FROM_BUFFER});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildLci() {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, MicroOp::LOAD_C_FROM_BUFFER});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildLdq() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::LOAD_Q_FROM_BUFFER});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildLan() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::LOAD_ACC_NEGATE_BUFFER});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildLcc() {
  auto ops = BuildAddressOnlySequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::COPY_MEM_TO_MEM_PLUS_ONE});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildSta() {
  auto ops = BuildAddressOnlySequence();
  ops.push_back({4, ClockPhase::CP2, MicroOp::ACC_TO_Z});
  ops.push_back({4, ClockPhase::CP3, MicroOp::Z_TO_BUFFER});
  ops.push_back({5, ClockPhase::CP1, MicroOp::BUFFER_TO_Y});
  ops.push_back({5, ClockPhase::CP2, MicroOp::Y_TO_MEM});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildStx() {
  auto ops = BuildAddressOnlySequence();
  ops.push_back({4, ClockPhase::CP2, MicroOp::X_TO_Z});
  ops.push_back({4, ClockPhase::CP3, MicroOp::Z_TO_BUFFER});
  ops.push_back({5, ClockPhase::CP1, MicroOp::BUFFER_TO_Y});
  ops.push_back({5, ClockPhase::CP2, MicroOp::Y_TO_MEM});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildStq() {
  auto ops = BuildAddressOnlySequence();
  ops.push_back({4, ClockPhase::CP2, MicroOp::Q_TO_Z});
  ops.push_back({4, ClockPhase::CP3, MicroOp::Z_TO_BUFFER});
  ops.push_back({5, ClockPhase::CP1, MicroOp::BUFFER_TO_Y});
  ops.push_back({5, ClockPhase::CP2, MicroOp::Y_TO_MEM});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildAdd() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::ACC_TO_Y});
  ops.push_back({5, ClockPhase::CP2, MicroOp::BUFFER_TO_X});
  ops.push_back({5, ClockPhase::CP3, MicroOp::ALU_ADD_TO_F});
  ops.push_back({6, ClockPhase::CP1, MicroOp::F_TO_ACCUMULATOR});
  ops.push_back({6, ClockPhase::CP2, MicroOp::UPDATE_OVERFLOW});
  ops.push_back({6, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildSub() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::ACC_TO_Y});
  ops.push_back({5, ClockPhase::CP2, MicroOp::BUFFER_TO_X});
  ops.push_back({5, ClockPhase::CP3, MicroOp::ALU_SUB_TO_F});
  ops.push_back({6, ClockPhase::CP1, MicroOp::F_TO_ACCUMULATOR});
  ops.push_back({6, ClockPhase::CP2, MicroOp::UPDATE_OVERFLOW});
  ops.push_back({6, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildMpy() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::MULTIPLY});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS_AQ});
  return ops;
}

std::vector<MicroOpStep> BuildDiv() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::DIVIDE});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS_Q});
  return ops;
}

std::vector<MicroOpStep> BuildRao() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::RAO});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildRso() {
  auto ops = BuildMemoryOperandSequence();
  ops.push_back({5, ClockPhase::CP1, MicroOp::RSO});
  ops.push_back({5, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildBranch() {
  auto ops = BuildAddressOnlySequence();
  ops.push_back({4, ClockPhase::CP1, MicroOp::BRANCH});
  ops.push_back({4, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildShift(MicroOp shift_op) {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, shift_op});
  if (shift_op == MicroOp::SHIFT_SLA || shift_op == MicroOp::SHIFT_SRA) {
    ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS_AQ});
  } else {
    ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  }
  return ops;
}

std::vector<MicroOpStep> BuildLogic(MicroOp op) {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, op});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildInx() {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, MicroOp::INCREMENT_X_BY_BUFFER});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildSkip(MicroOp op) {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, op});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildFlag(bool set) {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, set ? MicroOp::FLAG_SET : MicroOp::FLAG_CLEAR});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildSst() {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, MicroOp::SENSE_STATUS});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildOcd() {
  auto ops = BuildImmediateFetch();
  ops.push_back({3, ClockPhase::CP2, MicroOp::IO_NOOP});
  ops.push_back({3, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

std::vector<MicroOpStep> BuildIoMemory() {
  auto ops = BuildAddressOnlySequence();
  ops.push_back({4, ClockPhase::CP1, MicroOp::IO_NOOP});
  ops.push_back({4, ClockPhase::CP3, MicroOp::UPDATE_FLAGS});
  return ops;
}

}  // namespace

const std::vector<MicroOpStep>& MicrocodeTable::Acquisition() {
  static const std::vector<MicroOpStep> kAcquisition = BuildAcquisition();
  return kAcquisition;
}

const std::vector<MicroOpStep>& MicrocodeTable::Execution(uint8_t opcode) {
  static const std::vector<MicroOpStep> kSst = BuildSst();
  static const std::vector<MicroOpStep> kLci = BuildLci();
  static const std::vector<MicroOpStep> kLai = BuildLai();
  static const std::vector<MicroOpStep> kInx = BuildInx();
  static const std::vector<MicroOpStep> kSki = BuildSkip(MicroOp::SKIP_IF_INTERRUPT);
  static const std::vector<MicroOpStep> kSks = BuildSkip(MicroOp::SKIP_IF_SENSE);
  static const std::vector<MicroOpStep> kSkf = BuildSkip(MicroOp::SKIP_IF_FLAG);
  static const std::vector<MicroOpStep> kSla = BuildShift(MicroOp::SHIFT_SLA);
  static const std::vector<MicroOpStep> kSra = BuildShift(MicroOp::SHIFT_SRA);
  static const std::vector<MicroOpStep> kOcd = BuildOcd();
  static const std::vector<MicroOpStep> kLxi = BuildLxi();
  static const std::vector<MicroOpStep> kSll = BuildShift(MicroOp::SHIFT_SLL);
  static const std::vector<MicroOpStep> kSrl = BuildShift(MicroOp::SHIFT_SRL);
  static const std::vector<MicroOpStep> kAnd = BuildLogic(MicroOp::ALU_AND);
  static const std::vector<MicroOpStep> kIor = BuildLogic(MicroOp::ALU_IOR);
  static const std::vector<MicroOpStep> kXor = BuildLogic(MicroOp::ALU_XOR);
  static const std::vector<MicroOpStep> kFlc = BuildFlag(false);
  static const std::vector<MicroOpStep> kFls = BuildFlag(true);
  static const std::vector<MicroOpStep> kLda = BuildLda();
  static const std::vector<MicroOpStep> kLcc = BuildLcc();
  static const std::vector<MicroOpStep> kLan = BuildLan();
  static const std::vector<MicroOpStep> kLdq = BuildLdq();
  static const std::vector<MicroOpStep> kSta = BuildSta();
  static const std::vector<MicroOpStep> kStx = BuildStx();
  static const std::vector<MicroOpStep> kStq = BuildStq();
  static const std::vector<MicroOpStep> kAdd = BuildAdd();
  static const std::vector<MicroOpStep> kSub = BuildSub();
  static const std::vector<MicroOpStep> kMpy = BuildMpy();
  static const std::vector<MicroOpStep> kDiv = BuildDiv();
  static const std::vector<MicroOpStep> kRao = BuildRao();
  static const std::vector<MicroOpStep> kRso = BuildRso();
  static const std::vector<MicroOpStep> kBranch = BuildBranch();
  static const std::vector<MicroOpStep> kIoMem = BuildIoMemory();
  static const std::vector<MicroOpStep> kEmpty;

  switch (opcode) {
    case 0x00:
      return kSst;
    case 0x01:
      return kLci;
    case 0x02:
      return kLai;
    case 0x03:
      return kInx;
    case 0x08:
      return kSki;
    case 0x09:
      return kSks;
    case 0x0A:
      return kSkf;
    case 0x0B:
      return kSla;
    case 0x10:
      return kSra;
    case 0x11:
      return kOcd;
    case 0x12:
      return kLxi;
    case 0x13:
      return kSll;
    case 0x18:
      return kSrl;
    case 0x19:
      return kAnd;
    case 0x1A:
      return kIor;
    case 0x1B:
      return kXor;
    case 0x28:
      return kFlc;
    case 0xF8:
      return kFls;
    default:
      break;
  }

  uint8_t base = static_cast<uint8_t>(opcode & 0xF8);
  switch (base) {
    case 0x20:
      return kLda;
    case 0x30:
      return kLcc;
    case 0x38:
      return kLan;
    case 0x40:
      return kLdq;
    case 0x48:
      return kSta;
    case 0x50:
      return kStx;
    case 0x58:
      return kStq;
    case 0x60:
      return kAdd;
    case 0x68:
      return kSub;
    case 0x70:
      return kMpy;
    case 0x78:
      return kDiv;
    case 0x80:
      return kRao;
    case 0x88:
      return kRso;
    case 0x90:
    case 0x98:
    case 0xA0:
    case 0xA8:
    case 0xB0:
    case 0xB8:
    case 0xC0:
    case 0xC8:
      return kBranch;
    case 0xD0:
    case 0xD8:
    case 0xE0:
    case 0xE8:
    case 0xF0:
      return kIoMem;
    default:
      return kEmpty;
  }
}

}  // namespace ct10::core
