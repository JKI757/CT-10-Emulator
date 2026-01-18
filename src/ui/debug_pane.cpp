#include "ui/debug_pane.h"

#include <algorithm>

#include "imgui.h"
#include "core/microcode.h"

namespace ct10::ui {

namespace {

const char* PhaseLabel(core::ClockPhase phase) {
  switch (phase) {
    case core::ClockPhase::CP1:
      return "CP1";
    case core::ClockPhase::CP2:
      return "CP2";
    case core::ClockPhase::CP3:
      return "CP3";
  }
  return "?";
}

void DrawBus(const core::Bus& bus) {
  ImGui::Text("%s: %s 0x%04X", bus.name().c_str(),
              bus.driven() ? "DRIVEN" : "idle",
              static_cast<unsigned>(bus.value()));
}

const char* MicroOpLabel(core::MicroOp op) {
  switch (op) {
    case core::MicroOp::PAR_TO_MAR:
      return "PAR_TO_MAR";
    case core::MicroOp::MEM_TO_Z:
      return "MEM_TO_Z";
    case core::MicroOp::Z_TO_BUFFER:
      return "Z_TO_BUFFER";
    case core::MicroOp::BUFFER_TO_OPCODE:
      return "BUFFER_TO_OPCODE";
    case core::MicroOp::PAR_INC:
      return "PAR_INC";
    case core::MicroOp::FORM_EFFECTIVE_ADDRESS:
      return "FORM_EA";
    case core::MicroOp::ADD_INDEX_TO_MAR:
      return "ADD_INDEX";
    case core::MicroOp::MAR_TO_PAR:
      return "MAR_TO_PAR";
    case core::MicroOp::ACC_TO_Y:
      return "ACC_TO_Y";
    case core::MicroOp::BUFFER_TO_X:
      return "BUFFER_TO_X";
    case core::MicroOp::BUFFER_TO_F:
      return "BUFFER_TO_F";
    case core::MicroOp::F_TO_ACCUMULATOR:
      return "F_TO_A";
    case core::MicroOp::ACC_TO_Z:
      return "ACC_TO_Z";
    case core::MicroOp::X_TO_Z:
      return "X_TO_Z";
    case core::MicroOp::Q_TO_Z:
      return "Q_TO_Z";
    case core::MicroOp::BUFFER_TO_Y:
      return "BUFFER_TO_Y";
    case core::MicroOp::Y_TO_MEM:
      return "Y_TO_MEM";
    case core::MicroOp::LOAD_ACC_FROM_BUFFER:
      return "LOAD_A";
    case core::MicroOp::LOAD_X_FROM_BUFFER:
      return "LOAD_X";
    case core::MicroOp::LOAD_C_FROM_BUFFER:
      return "LOAD_C";
    case core::MicroOp::LOAD_Q_FROM_BUFFER:
      return "LOAD_Q";
    case core::MicroOp::LOAD_ACC_NEGATE_BUFFER:
      return "LOAD_A_NEG";
    case core::MicroOp::STORE_ACC_TO_MEM:
      return "STORE_A";
    case core::MicroOp::STORE_X_TO_MEM:
      return "STORE_X";
    case core::MicroOp::STORE_Q_TO_MEM:
      return "STORE_Q";
    case core::MicroOp::COPY_MEM_TO_MEM_PLUS_ONE:
      return "LCC_COPY";
    case core::MicroOp::INCREMENT_X_BY_BUFFER:
      return "INX";
    case core::MicroOp::ALU_ADD_TO_F:
      return "ALU_ADD";
    case core::MicroOp::ALU_SUB_TO_F:
      return "ALU_SUB";
    case core::MicroOp::ALU_AND:
      return "ALU_AND";
    case core::MicroOp::ALU_IOR:
      return "ALU_IOR";
    case core::MicroOp::ALU_XOR:
      return "ALU_XOR";
    case core::MicroOp::SHIFT_SLA:
      return "SHIFT_SLA";
    case core::MicroOp::SHIFT_SRA:
      return "SHIFT_SRA";
    case core::MicroOp::SHIFT_SLL:
      return "SHIFT_SLL";
    case core::MicroOp::SHIFT_SRL:
      return "SHIFT_SRL";
    case core::MicroOp::MULTIPLY:
      return "MULTIPLY";
    case core::MicroOp::DIVIDE:
      return "DIVIDE";
    case core::MicroOp::RAO:
      return "RAO";
    case core::MicroOp::RSO:
      return "RSO";
    case core::MicroOp::BRANCH:
      return "BRANCH";
    case core::MicroOp::SKIP_IF_INTERRUPT:
      return "SKIP_INT";
    case core::MicroOp::SKIP_IF_SENSE:
      return "SKIP_SENSE";
    case core::MicroOp::SKIP_IF_FLAG:
      return "SKIP_FLAG";
    case core::MicroOp::FLAG_SET:
      return "FLAG_SET";
    case core::MicroOp::FLAG_CLEAR:
      return "FLAG_CLEAR";
    case core::MicroOp::SENSE_STATUS:
      return "SENSE_STATUS";
    case core::MicroOp::IO_NOOP:
      return "IO_NOOP";
    case core::MicroOp::ALU_DIV:
      return "ALU_DIV";
    case core::MicroOp::ALU_MUL:
      return "ALU_MUL";
    case core::MicroOp::UPDATE_FLAGS:
      return "FLAGS_A";
    case core::MicroOp::UPDATE_FLAGS_Q:
      return "FLAGS_Q";
    case core::MicroOp::UPDATE_FLAGS_AQ:
      return "FLAGS_AQ";
    case core::MicroOp::UPDATE_OVERFLOW:
      return "OVF";
    case core::MicroOp::HALT:
      return "HALT";
  }
  return "?";
}

}  // namespace

void DebugPane::Draw(const core::MachineState& state, float top_offset) const {
  ImVec2 display = ImGui::GetIO().DisplaySize;
  float width = 340.0f;
  float max_top = display.y - 120.0f - 20.0f;
  float top = std::min(top_offset, max_top);
  if (top < 20.0f) {
    top = 20.0f;
  }
  float height = display.y - top - 20.0f;
  if (height < 120.0f) {
    height = 120.0f;
  }
  float pos_x = display.x - width - 20.0f;
  if (pos_x < 20.0f) {
    pos_x = 20.0f;
  }
  ImGui::SetNextWindowPos(ImVec2(pos_x, top), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

  ImGui::Begin("Debug");

  ImGui::Text("Timing: D=%u %s %s",
              static_cast<unsigned>(state.timing.distributor),
              PhaseLabel(state.timing.phase),
              state.timing.acquisition ? "acq" : "exec");

  ImGui::Separator();
  ImGui::Text("Registers");
  ImGui::Text("A:   0x%02X", static_cast<unsigned>(state.accumulator.value()));
  ImGui::Text("B:   0x%02X", static_cast<unsigned>(state.buffer.value()));
  ImGui::Text("Q:   0x%02X", static_cast<unsigned>(state.quotient.value()));
  ImGui::Text("X:   0x%02X", static_cast<unsigned>(state.index.value()));
  ImGui::Text("C:   0x%02X", static_cast<unsigned>(state.countdown.value()));
  ImGui::Text("MAR: 0x%03X", static_cast<unsigned>(state.mar.value()));
  ImGui::Text("PAR: 0x%03X", static_cast<unsigned>(state.par.value()));
  ImGui::Text("OP:  0x%02X", static_cast<unsigned>(state.opcode.value()));
  ImGui::Text("D:   0x%01X", static_cast<unsigned>(state.distributor.value()));

  ImGui::Separator();
  ImGui::Text("Flags: C=%d Z=%d GT=%d LT=%d AO=%d DO=%d IE=%d",
              state.flags.carry,
              state.flags.zero,
              state.flags.greater,
              state.flags.less,
              state.flags.add_overflow,
              state.flags.divide_overflow,
              state.flags.inst_error);

  ImGui::Separator();
  ImGui::Text("Buses");
  DrawBus(state.x_bus);
  DrawBus(state.y_bus);
  DrawBus(state.z_bus);
  DrawBus(state.f_bus);

  ImGui::Separator();
  ImGui::Text("Trace");
  size_t trace_count = state.trace.size();
  size_t show = std::min<size_t>(trace_count, 12);
  for (size_t i = 0; i < show; ++i) {
    const auto& entry = state.trace[trace_count - show + i];
    ImGui::Text("D%u %s %s %s",
                static_cast<unsigned>(entry.distributor),
                PhaseLabel(entry.phase),
                entry.acquisition ? "A" : "E",
                MicroOpLabel(entry.op));
  }

  ImGui::Separator();
  ImGui::Text("Panel Input");
  if (state.panel_input.has_last_key) {
    ImGui::Text("Last key: 0x%X", static_cast<unsigned>(state.panel_input.last_key));
  } else {
    ImGui::Text("Last key: --");
  }
  ImGui::Text("Key event: %s", state.panel_input.key_pressed ? "yes" : "no");
  ImGui::Text("START: %s  CLEAR: %s  LAMP: %s",
              state.panel_input.start ? "yes" : "no",
              state.panel_input.clear ? "yes" : "no",
              state.panel_input.lamp_test ? "yes" : "no");
  ImGui::Text("STOP: %s  RESET: %s  POWER: %s",
              state.panel_input.stop ? "yes" : "no",
              state.panel_input.reset ? "yes" : "no",
              state.panel_input.power_on ? "on" : "off");
  ImGui::Text("Input: 0x%03X  I/O mode: %u  Mode: %u",
              static_cast<unsigned>(state.panel_input.input_switches),
              static_cast<unsigned>(state.panel_input.io_mode),
              static_cast<unsigned>(state.panel_input.mode));
  ImGui::Text("Mem: RD=%s WT=%s",
              state.panel_input.mem_read ? "yes" : "no",
              state.panel_input.mem_write ? "yes" : "no");

  ImGui::Separator();
  ImGui::Text("I/O");
  auto transfer_label = [](core::IoTransferMode mode) -> const char* {
    switch (mode) {
      case core::IoTransferMode::ManualOutput:
        return "manual-out";
      case core::IoTransferMode::ManualInput:
        return "manual-in";
      case core::IoTransferMode::ReadBlock:
        return "read-block";
      case core::IoTransferMode::WriteBlock:
        return "write-block";
      case core::IoTransferMode::ReadInterrupt:
        return "read-intrp";
      case core::IoTransferMode::None:
      default:
        return "none";
    }
  };
  ImGui::Text("Status: 0x%02X  Cmd: 0x%02X  IRQ: %s",
              static_cast<unsigned>(state.io.status),
              static_cast<unsigned>(state.io.last_command),
              state.io.interrupt ? "yes" : "no");
  ImGui::Text("Dev: %u  HEX: %s  ALPHA: %s",
              static_cast<unsigned>(state.io.selected_device),
              state.io.hex_mode ? "yes" : "no",
              state.io.alpha_mode ? "yes" : "no");
  ImGui::Text("Transfer: %s  Addr: 0x%03X  Rem: %u  Wait: %u",
              transfer_label(state.io.transfer_mode),
              static_cast<unsigned>(state.io.transfer_address),
              static_cast<unsigned>(state.io.transfer_remaining),
              static_cast<unsigned>(state.io.wait_cycles));
  ImGui::Text("Tape: in %zu  out %zu",
              state.io.input_data.size(),
              state.io.output_data.size());
  ImGui::Text("Term: in %zu  out %zu",
              state.io.terminal_input.size(),
              state.io.terminal_output.size());
  ImGui::Text("Printer: out %zu",
              state.io.printer_output.size());

  ImGui::End();
}

}  // namespace ct10::ui
