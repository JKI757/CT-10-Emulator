#include "core/machine_state.h"

namespace ct10::core {

MachineState::MachineState()
    : accumulator("A", 8),
      buffer("B", 8),
      quotient("Q", 8),
      index("X", 8),
      countdown("C", 8),
      mar("MAR", 10),
      par("PAR", 10),
      opcode("OP", 8),
      distributor("D", 4),
      x_bus("X"),
      y_bus("Y"),
      z_bus("Z"),
      f_bus("F") {
  Reset();
}

void MachineState::Reset() {
  accumulator.Clear();
  buffer.Clear();
  quotient.Clear();
  index.Clear();
  countdown.Clear();
  mar.Clear();
  par.Clear();
  opcode.Clear();
  distributor.Clear();

  x_bus.Clear();
  y_bus.Clear();
  z_bus.Clear();
  f_bus.Clear();

  flags = {};
  timing = {};
  mode = {};
  status = {};
  io = {};
  panel_input = {};
  trace.clear();
}

void MachineState::ClearRegisters() {
  accumulator.Clear();
  buffer.Clear();
  quotient.Clear();
  index.Clear();
  countdown.Clear();
  mar.Clear();
  par.Clear();
  opcode.Clear();
  distributor.Clear();

  x_bus.Clear();
  y_bus.Clear();
  z_bus.Clear();
  f_bus.Clear();

  flags = {};
  timing = {};
  mode = {};
  status = {};
  trace.clear();
}

void MachineState::AddTrace(MicroOp op) {
  TraceEntry entry;
  entry.distributor = timing.distributor;
  entry.phase = timing.phase;
  entry.acquisition = timing.acquisition;
  entry.op = op;
  if (trace.size() >= kTraceCapacity) {
    trace.erase(trace.begin());
  }
  trace.push_back(entry);
}

void MachineState::ClearTrace() {
  trace.clear();
}

}  // namespace ct10::core
