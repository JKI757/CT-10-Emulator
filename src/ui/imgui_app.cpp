#include "ui/imgui_app.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include <GLFW/glfw3.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include "app/golden_program.h"
#include "app/program_text.h"
#include "app/tape_io.h"
#include "core/state_io.h"
#include "ui/debug_pane.h"
#include "ui/panel_layout.h"
#include "ui/panel_view.h"

namespace ct10::ui {
namespace {

constexpr float kRightPaneMargin = 20.0f;
constexpr float kRightPaneWidth = 260.0f;
constexpr float kControlsHeight = 200.0f;
constexpr float kRightPaneGap = 20.0f;
constexpr float kProgramHeight = 520.0f;
constexpr float kProgramTop = kRightPaneMargin + kControlsHeight + kRightPaneGap;
constexpr float kDebugTop = kProgramTop + kProgramHeight + kRightPaneGap;
constexpr int kPanelStepGuard = 200000;
constexpr size_t kIoTextMaxBytes = 4096;

int ClampMaxSteps(int steps) {
  if (steps < 1) {
    return 1;
  }
  if (steps > 10000000) {
    return 10000000;
  }
  return steps;
}

struct PanelFonts {
  ImFont* ui = nullptr;
  ImFont* input = nullptr;
  ImFont* display = nullptr;
};

std::string RenderAscii(const std::vector<uint8_t>& data, size_t max_bytes) {
  size_t start = data.size() > max_bytes ? data.size() - max_bytes : 0;
  std::string out;
  out.reserve(data.size() - start);
  for (size_t i = start; i < data.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(data[i]);
    if (c == '\r') {
      continue;
    }
    if (c == '\n' || c == '\t') {
      out.push_back(static_cast<char>(c));
    } else if (c >= 32 && c < 127) {
      out.push_back(static_cast<char>(c));
    } else {
      out.push_back('.');
    }
  }
  return out;
}

std::string AppendTimestamp(const std::string& path) {
  if (path.empty()) {
    return path;
  }
  size_t slash = path.find_last_of("/\\");
  size_t dot = path.find_last_of('.');
  bool has_ext = dot != std::string::npos &&
                 (slash == std::string::npos || dot > slash);
  std::string base = has_ext ? path.substr(0, dot) : path;
  std::string ext = has_ext ? path.substr(dot) : "";

  std::time_t now = std::time(nullptr);
  std::tm tm_snapshot{};
#if defined(_WIN32)
  localtime_s(&tm_snapshot, &now);
#else
  localtime_r(&now, &tm_snapshot);
#endif
  char stamp[32] = {};
  std::strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", &tm_snapshot);
  return base + "_" + stamp + ext;
}

ImFont* LoadFontOption(size_t index) {
  ImGuiIO& io = ImGui::GetIO();
  if (index >= PanelLayout::kFontOptions.size()) {
    index = 0;
  }
  const auto& option = PanelLayout::kFontOptions[index];
  ImFontConfig config;
  config.SizePixels = option.size;
  ImFont* font = nullptr;
  if (option.path && option.path[0] != '\0') {
    std::error_code ec;
    if (std::filesystem::exists(option.path, ec)) {
      font = io.Fonts->AddFontFromFileTTF(option.path, option.size);
    }
  }
  if (!font) {
    font = io.Fonts->AddFontDefault(&config);
  }
  if (font) {
    font->Scale = option.scale;
  }
  return font;
}

PanelFonts LoadPanelFonts() {
  ImGuiIO& io = ImGui::GetIO();
  io.FontGlobalScale = 1.0f;

  PanelFonts fonts;
  fonts.ui = LoadFontOption(PanelLayout::kUiFontOptionIndex);
  fonts.input = LoadFontOption(PanelLayout::kInputFontOptionIndex);
  fonts.display = LoadFontOption(PanelLayout::kDisplayFontOptionIndex);

  io.FontDefault = fonts.ui ? fonts.ui : (fonts.input ? fonts.input : fonts.display);
  return fonts;
}

void StepClock(core::TimingEngine& timing,
               core::MachineState& state,
               core::ExecutionEngine& execution) {
  bool was_halted = state.mode.halted;
  state.mode.halted = false;
  execution.Step(state);
  if (!state.mode.halted) {
    state.mode.halted = was_halted;
  }
  timing.Advance(state.timing);
}

void StepDistributor(core::TimingEngine& timing,
                     core::MachineState& state,
                     core::ExecutionEngine& execution) {
  uint8_t start = state.timing.distributor;
  do {
    StepClock(timing, state, execution);
  } while (state.timing.distributor == start);
}

void StepUntilPhaseToggle(core::TimingEngine& timing,
                          core::MachineState& state,
                          core::ExecutionEngine& execution) {
  bool start_acq = state.timing.acquisition;
  int guard = 0;
  while (guard < kPanelStepGuard) {
    StepClock(timing, state, execution);
    if (state.timing.acquisition != start_acq) {
      break;
    }
    ++guard;
  }
}

void StepInstruction(core::TimingEngine& timing,
                     core::MachineState& state,
                     core::ExecutionEngine& execution) {
  int toggles = 0;
  bool start_acq = state.timing.acquisition;
  int guard = 0;
  while (guard < kPanelStepGuard && toggles < 2) {
    StepClock(timing, state, execution);
    if (state.timing.acquisition != start_acq) {
      start_acq = state.timing.acquisition;
      ++toggles;
    }
    ++guard;
  }
}

void RunGoldenTest(const ImGuiApp::ResetHook& reset_hook,
                   int max_steps,
                   std::string& message,
                   bool& passed) {
  if (!reset_hook) {
    message = "No program loader available.";
    passed = false;
    return;
  }

  core::MachineState test_state;
  core::TimingEngine test_timing;
  core::ExecutionEngine test_execution;

  reset_hook(test_state);
  test_timing.Reset(test_state.timing);

  max_steps = ClampMaxSteps(max_steps);
  int steps = 0;
  for (; steps < max_steps; ++steps) {
    StepClock(test_timing, test_state, test_execution);
    if (test_state.mode.halted) {
      break;
    }
  }

  if (!test_state.mode.halted) {
    message = "Golden test did not halt.";
    passed = false;
    return;
  }

  uint8_t result =
      test_state.memory.Read(app::kGoldenProgramResultAddress);
  if (result != app::kGoldenProgramExpectedValue) {
    std::ostringstream out;
    out << "Golden test failed: mem[0x"
        << std::hex << std::uppercase
        << static_cast<int>(app::kGoldenProgramResultAddress)
        << "] = 0x" << static_cast<int>(result);
    message = out.str();
    passed = false;
    return;
  }

  std::ostringstream out;
  out << "Golden test passed in " << (steps + 1) << " clock steps.";
  message = out.str();
  passed = true;
}

void DrawControls(core::MachineState& state,
                  core::TimingEngine& timing,
                  app::ModeController& mode,
                  const ImGuiApp::ResetHook& reset_hook,
                  const ImVec2& display_size,
  bool& step_clock,
  bool& step_distributor) {
  ImVec2 pos(display_size.x - kRightPaneWidth - kRightPaneMargin,
             kRightPaneMargin);
  if (pos.x < kRightPaneMargin) {
    pos.x = kRightPaneMargin;
  }
  ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(kRightPaneWidth, kControlsHeight),
                           ImGuiCond_Always);
  ImGui::Begin("Controls");

  if (ImGui::Button("Run")) {
    mode.SetMode(app::RunMode::Continuous);
  }
  ImGui::SameLine();
  if (ImGui::Button("Halt")) {
    mode.SetMode(app::RunMode::Halted);
  }

  if (ImGui::Button("Step Clock")) {
    step_clock = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Step Dist")) {
    step_distributor = true;
  }

  if (ImGui::Button("Reload Program") && reset_hook) {
    reset_hook(state);
    timing.Reset(state.timing);
    mode.SetMode(app::RunMode::Halted);
  }

  float speed = static_cast<float>(timing.speed_multiplier());
  if (ImGui::SliderFloat("Speed", &speed, 1.0f, 10.0f, "%.1fx")) {
    timing.set_speed_multiplier(speed);
  }

  ImGui::Text("Mode: %s", mode.IsHalted() ? "halted" : "running");
  ImGui::Text("Panel: %dx%d", PanelLayout::kWidth, PanelLayout::kHeight);

  ImGui::Separator();
  ImGui::Text("Debug");
  static int max_steps = 200000;
  static std::string golden_message;
  static bool golden_passed = false;
  ImGui::InputInt("Max Steps", &max_steps);
  max_steps = ClampMaxSteps(max_steps);
  if (ImGui::Button("Run Golden Test")) {
    RunGoldenTest(reset_hook, max_steps, golden_message, golden_passed);
  }
  if (!golden_message.empty()) {
    ImVec4 color =
        golden_passed ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                      : ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(color, "%s", golden_message.c_str());
  }

  ImGui::End();
}

void DrawProgramEditor(core::MachineState& state,
                       app::ModeController& mode,
                       const ImVec2& display_size) {
  ImVec2 pos(display_size.x - kRightPaneWidth - kRightPaneMargin,
             kProgramTop);
  if (pos.x < kRightPaneMargin) {
    pos.x = kRightPaneMargin;
  }
  ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(kRightPaneWidth, kProgramHeight),
                           ImGuiCond_Always);
  ImGui::Begin("Program");

  static char program_text[8192] = "";
  static int start_address = 0;
  static bool clear_memory = false;
  static bool set_par = true;
  static std::string load_message;
  static bool load_ok = true;
  static char tape_in_path[256] = "";
  static char tape_out_path[256] = "";
  static char state_path[256] = "";
  static char terminal_out_path[256] = "";
  static char printer_out_path[256] = "";
  static std::string tape_message;
  static bool tape_ok = true;
  static std::string terminal_message;
  static bool terminal_ok = true;
  static std::string printer_message;
  static bool printer_ok = true;
  static bool terminal_save_timestamp = false;
  static bool printer_save_timestamp = false;
  static bool terminal_save_newline = true;
  static bool printer_save_newline = true;
  static int terminal_save_format = 1;
  static int printer_save_format = 1;
  static std::string state_message;
  static bool state_ok = true;
  static std::string expect_message;
  static bool expect_ok = true;

  ImGui::InputInt("Start Addr", &start_address);
  start_address = std::clamp(
      start_address, 0, static_cast<int>(core::Memory::kSize - 1));
  ImGui::Checkbox("Clear memory", &clear_memory);
  ImGui::SameLine();
  ImGui::Checkbox("Set PAR", &set_par);

  ImGui::InputTextMultiline("##program_text", program_text,
                            sizeof(program_text), ImVec2(-1.0f, 190.0f));
  ImGui::TextDisabled("Directives: @ADDR, # START <addr>, # EXPECT <addr> <val>");

  if (ImGui::Button("Load Program")) {
    app::ProgramSpec spec;
    app::ParseResult result;
    app::ParseProgramContent(program_text, spec, result);
    if (spec.writes.empty()) {
      load_message = "No bytes parsed.";
      load_ok = false;
    } else {
      if (clear_memory) {
        state.memory.Clear();
      }
      int loaded = 0;
      bool overflow = false;
      for (const auto& write : spec.writes) {
        uint16_t address = write.address;
        if (!spec.uses_addresses) {
          address = static_cast<uint16_t>(address + start_address);
        }
        if (address >= core::Memory::kSize) {
          overflow = true;
          break;
        }
        state.memory.Write(address, write.value);
        ++loaded;
      }
      if (set_par) {
        uint16_t entry = 0;
        if (spec.has_entry) {
          entry = spec.entry;
        } else if (spec.uses_addresses && !spec.writes.empty()) {
          entry = spec.writes.front().address;
        } else {
          entry = static_cast<uint16_t>(start_address);
        }
        state.par.Load(entry);
      }

      std::ostringstream out;
      out << "Loaded " << loaded << " byte";
      if (loaded != 1) {
        out << "s";
      }
      if (spec.uses_addresses && start_address != 0) {
        out << " (start addr ignored)";
      }
      if (result.skipped > 0) {
        out << ", skipped " << result.skipped;
      }
      if (!spec.expects.empty()) {
        out << ", expects " << spec.expects.size();
      }
      if (overflow) {
        out << " (truncated)";
      }
      out << ".";
      load_message = out.str();
      load_ok = !overflow;
    }
  }

  if (!load_message.empty()) {
    ImVec4 color =
        load_ok ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                : ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(color, "%s", load_message.c_str());
  }

  if (ImGui::Button("Verify EXPECT")) {
    app::ProgramSpec spec;
    app::ParseResult result;
    app::ParseProgramContent(program_text, spec, result);
    if (spec.expects.empty()) {
      expect_message = "No EXPECT directives.";
      expect_ok = false;
    } else {
      bool ok = true;
      for (const auto& expect : spec.expects) {
        uint8_t value = state.memory.Read(expect.address);
        if (value != expect.value) {
          std::ostringstream out;
          out << "EXPECT failed at 0x" << std::hex << std::uppercase
              << static_cast<unsigned>(expect.address)
              << ": 0x" << static_cast<unsigned>(value)
              << " (expected 0x" << static_cast<unsigned>(expect.value) << ")";
          expect_message = out.str();
          ok = false;
          break;
        }
      }
      if (ok) {
        std::ostringstream out;
        out << "EXPECT passed (" << spec.expects.size() << ").";
        expect_message = out.str();
      }
      expect_ok = ok;
    }
  }
  if (!expect_message.empty()) {
    ImVec4 color =
        expect_ok ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                  : ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(color, "%s", expect_message.c_str());
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Trace")) {
    state.ClearTrace();
  }

  ImGui::Separator();
  ImGui::Text("I/O Tape");
  static bool io_override_device = false;
  static int io_device = 0;
  const char* io_devices[] = {"Tape (0)", "Terminal (1)", "Printer (2)"};
  ImGui::Checkbox("Override device", &io_override_device);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(140.0f);
  ImGui::Combo("##io_device", &io_device, io_devices,
               IM_ARRAYSIZE(io_devices));
  if (io_override_device) {
    state.io.selected_device = static_cast<uint8_t>(io_device);
  }
  ImGui::InputText("Load Path", tape_in_path, sizeof(tape_in_path));
  if (ImGui::Button("Load Tape")) {
    switch (state.panel_input.io_mode) {
      case 2:
        state.io.hex_mode = true;
        state.io.alpha_mode = false;
        break;
      case 3:
        state.io.hex_mode = false;
        state.io.alpha_mode = true;
        break;
      default:
        state.io.hex_mode = false;
        state.io.alpha_mode = false;
        break;
    }
    std::string error;
    if (app::LoadTapeText(tape_in_path, state.io, &error)) {
      tape_message = "Tape loaded.";
      tape_ok = true;
      if (!error.empty()) {
        tape_message = error;
      }
    } else {
      tape_message = error.empty() ? "Tape load failed." : error;
      tape_ok = false;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Input")) {
    state.io.input_data.clear();
    state.io.input_pos = 0;
    state.io.interrupt = false;
    tape_message = "Input cleared.";
    tape_ok = true;
  }
  ImGui::InputText("Save Path", tape_out_path, sizeof(tape_out_path));
  if (ImGui::Button("Save Tape")) {
    switch (state.panel_input.io_mode) {
      case 2:
        state.io.hex_mode = true;
        state.io.alpha_mode = false;
        break;
      case 3:
        state.io.hex_mode = false;
        state.io.alpha_mode = true;
        break;
      default:
        state.io.hex_mode = false;
        state.io.alpha_mode = false;
        break;
    }
    std::string error;
    if (app::SaveTapeText(tape_out_path, state.io, &error)) {
      tape_message = "Tape saved.";
      tape_ok = true;
    } else {
      tape_message = error.empty() ? "Tape save failed." : error;
      tape_ok = false;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Output")) {
    state.io.output_data.clear();
    tape_message = "Output cleared.";
    tape_ok = true;
  }
  ImGui::Text("Input: %zu bytes  Output: %zu bytes",
              state.io.input_data.size(),
              state.io.output_data.size());
  if (!tape_message.empty()) {
    ImVec4 color =
        tape_ok ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                : ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(color, "%s", tape_message.c_str());
  }

  ImGui::Separator();
  ImGui::Text("Terminal");
  static char terminal_input[1024] = "";
  static bool terminal_append_newline = true;
  ImGui::InputTextMultiline("##terminal_input", terminal_input,
                            sizeof(terminal_input), ImVec2(-1.0f, 70.0f));
  if (ImGui::Button("Send")) {
    size_t len = std::strlen(terminal_input);
    if (len > 0) {
      state.io.terminal_input.reserve(state.io.terminal_input.size() + len + 1);
      for (size_t i = 0; i < len; ++i) {
        state.io.terminal_input.push_back(
            static_cast<uint8_t>(terminal_input[i]));
      }
      if (terminal_append_newline) {
        state.io.terminal_input.push_back('\n');
      }
      state.io.interrupt = false;
      terminal_input[0] = '\0';
    }
  }
  ImGui::SameLine();
  ImGui::Checkbox("Append newline", &terminal_append_newline);
  ImGui::SameLine();
  if (ImGui::Button("Clear Term Input")) {
    state.io.terminal_input.clear();
    state.io.terminal_input_pos = 0;
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Term Output")) {
    state.io.terminal_output.clear();
  }
  ImGui::InputText("Terminal Save Path", terminal_out_path,
                   sizeof(terminal_out_path));
  const char* save_formats[] = {"Raw", "Text", "Hex"};
  ImGui::SetNextItemWidth(120.0f);
  ImGui::Combo("Term Format", &terminal_save_format, save_formats,
               IM_ARRAYSIZE(save_formats));
  ImGui::Checkbox("Term Timestamp", &terminal_save_timestamp);
  ImGui::SameLine();
  ImGui::Checkbox("Term Append NL", &terminal_save_newline);
  if (ImGui::Button("Save Term Output")) {
    if (terminal_out_path[0] == '\0') {
      terminal_message = "Terminal save path required.";
      terminal_ok = false;
    } else {
      std::string error;
      std::string save_path = terminal_save_timestamp
                                  ? AppendTimestamp(terminal_out_path)
                                  : std::string(terminal_out_path);
      bool ok = false;
      if (terminal_save_format == 0) {
        ok = app::SaveByteStream(save_path,
                                 state.io.terminal_output,
                                 &error,
                                 terminal_save_newline);
      } else if (terminal_save_format == 1) {
        ok = app::SaveAsciiText(save_path,
                                state.io.terminal_output,
                                &error,
                                terminal_save_newline);
      } else {
        ok = app::SaveHexDump(save_path,
                              state.io.terminal_output,
                              &error);
      }
      if (ok) {
        terminal_message = "Terminal output saved.";
        terminal_ok = true;
      } else {
        terminal_message = error.empty() ? "Terminal save failed." : error;
        terminal_ok = false;
      }
    }
  }
  ImGui::Text("Input: %zu bytes  Output: %zu bytes",
              state.io.terminal_input.size(),
              state.io.terminal_output.size());
  if (!terminal_message.empty()) {
    ImVec4 color =
        terminal_ok ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                    : ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(color, "%s", terminal_message.c_str());
  }
  ImGui::BeginChild("TerminalOutput", ImVec2(0.0f, 120.0f), true);
  std::string terminal_output = RenderAscii(state.io.terminal_output,
                                            kIoTextMaxBytes);
  ImGui::TextUnformatted(terminal_output.c_str());
  ImGui::EndChild();

  ImGui::Separator();
  ImGui::Text("Printer");
  if (ImGui::Button("Clear Printer Output")) {
    state.io.printer_output.clear();
  }
  ImGui::InputText("Printer Save Path", printer_out_path,
                   sizeof(printer_out_path));
  ImGui::SetNextItemWidth(120.0f);
  ImGui::Combo("Printer Format", &printer_save_format, save_formats,
               IM_ARRAYSIZE(save_formats));
  ImGui::Checkbox("Printer Timestamp", &printer_save_timestamp);
  ImGui::SameLine();
  ImGui::Checkbox("Printer Append NL", &printer_save_newline);
  if (ImGui::Button("Save Printer Output")) {
    if (printer_out_path[0] == '\0') {
      printer_message = "Printer save path required.";
      printer_ok = false;
    } else {
      std::string error;
      std::string save_path = printer_save_timestamp
                                  ? AppendTimestamp(printer_out_path)
                                  : std::string(printer_out_path);
      bool ok = false;
      if (printer_save_format == 0) {
        ok = app::SaveByteStream(save_path,
                                 state.io.printer_output,
                                 &error,
                                 printer_save_newline);
      } else if (printer_save_format == 1) {
        ok = app::SaveAsciiText(save_path,
                                state.io.printer_output,
                                &error,
                                printer_save_newline);
      } else {
        ok = app::SaveHexDump(save_path,
                              state.io.printer_output,
                              &error);
      }
      if (ok) {
        printer_message = "Printer output saved.";
        printer_ok = true;
      } else {
        printer_message = error.empty() ? "Printer save failed." : error;
        printer_ok = false;
      }
    }
  }
  ImGui::Text("Output: %zu bytes", state.io.printer_output.size());
  if (!printer_message.empty()) {
    ImVec4 color =
        printer_ok ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                   : ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(color, "%s", printer_message.c_str());
  }
  ImGui::BeginChild("PrinterOutput", ImVec2(0.0f, 120.0f), true);
  std::string printer_output = RenderAscii(state.io.printer_output,
                                           kIoTextMaxBytes);
  ImGui::TextUnformatted(printer_output.c_str());
  ImGui::EndChild();

  ImGui::Separator();
  ImGui::Text("State");
  ImGui::InputText("State Path", state_path, sizeof(state_path));
  if (ImGui::Button("Save State")) {
    std::string error;
    if (core::SaveState(state, state_path, &error)) {
      state_message = "State saved.";
      state_ok = true;
    } else {
      state_message = error.empty() ? "State save failed." : error;
      state_ok = false;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Load State")) {
    std::string error;
    if (core::LoadState(state, state_path, &error)) {
      state_message = "State loaded.";
      state_ok = true;
      mode.SetMode(state.mode.halted ? app::RunMode::Halted
                                     : app::RunMode::Continuous);
    } else {
      state_message = error.empty() ? "State load failed." : error;
      state_ok = false;
    }
  }
  if (!state_message.empty()) {
    ImVec4 color =
        state_ok ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                 : ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
    ImGui::TextColored(color, "%s", state_message.c_str());
  }

  ImGui::End();
}

}  // namespace

void ApplyPanelControls(core::MachineState& state,
                        core::TimingEngine& timing,
                        app::ModeController& mode,
                        const ImGuiApp::ResetHook& reset_hook) {
  static bool last_power_on = true;
  bool power_on = state.panel_input.power_on;

  if (power_on != last_power_on) {
    if (!power_on) {
      state.ClearRegisters();
      timing.Reset(state.timing);
      mode.SetMode(app::RunMode::Halted);
      state.mode.halted = true;
      state.io.transfer_mode = core::IoTransferMode::None;
      state.io.transfer_address = 0;
      state.io.transfer_remaining = 0;
      state.io.wait_cycles = 0;
      state.panel_input.key_value = 0;
      state.panel_input.last_key = 0;
      state.panel_input.has_last_key = false;
      state.panel_input.load_pressed = false;
      state.panel_input.load_target = core::LoadTarget::None;
    } else {
      timing.Reset(state.timing);
      mode.SetMode(app::RunMode::Halted);
      state.mode.halted = true;
      state.io.transfer_mode = core::IoTransferMode::None;
      state.io.transfer_address = 0;
      state.io.transfer_remaining = 0;
      state.io.wait_cycles = 0;
      state.panel_input.key_value = 0;
      state.panel_input.last_key = 0;
      state.panel_input.has_last_key = false;
      state.panel_input.load_pressed = false;
      state.panel_input.load_target = core::LoadTarget::None;
    }
    state.panel_input.ClearMomentary();
    last_power_on = power_on;
  }

  if (!power_on) {
    mode.SetMode(app::RunMode::Halted);
    state.mode.halted = true;
    return;
  }

  if (state.panel_input.reset && reset_hook) {
    reset_hook(state);
    timing.Reset(state.timing);
    mode.SetMode(app::RunMode::Halted);
    state.mode.halted = true;
    state.io.transfer_mode = core::IoTransferMode::None;
    state.io.transfer_address = 0;
    state.io.transfer_remaining = 0;
    state.io.wait_cycles = 0;
    state.panel_input.input_switches = 0;
    state.panel_input.key_value = 0;
    state.panel_input.last_key = 0;
    state.panel_input.has_last_key = false;
  }

  if (state.panel_input.clear) {
    state.ClearRegisters();
    timing.Reset(state.timing);
    mode.SetMode(app::RunMode::Halted);
    state.io.transfer_mode = core::IoTransferMode::None;
    state.io.transfer_address = 0;
    state.io.transfer_remaining = 0;
    state.io.wait_cycles = 0;
  }
}

void RefreshPanelStatus(core::MachineState& state) {
  state.status.sense = state.panel_input.sense;
  state.status.interrupt = state.io.interrupt;
}

void ApplyPanelIoPreset(core::MachineState& state) {
  static bool last_read_intrp = false;
  static bool last_write_block = false;

  bool read_intrp = state.panel_input.io_read && state.panel_input.io_intrp;
  bool write_block = state.panel_input.io_write && state.panel_input.io_block;

  if (!state.panel_input.power_on) {
    last_read_intrp = read_intrp;
    last_write_block = write_block;
    return;
  }

  if (read_intrp && !last_read_intrp) {
    state.opcode.Load(0xE8);
    state.countdown.Load(0xFF);
    state.timing.distributor = 0;
    state.timing.phase = core::ClockPhase::CP1;
    state.timing.acquisition = false;
    state.distributor.Load(state.timing.distributor);
  }

  if (write_block && !last_write_block) {
    state.opcode.Load(0xD0);
    state.countdown.Load(0xFF);
    state.timing.distributor = 0;
    state.timing.phase = core::ClockPhase::CP1;
    state.timing.acquisition = false;
    state.distributor.Load(state.timing.distributor);
  }

  last_read_intrp = read_intrp;
  last_write_block = write_block;
}

void ApplyRegisterLoad(core::MachineState& state) {
  if (!state.panel_input.power_on) {
    state.panel_input.load_pressed = false;
    state.panel_input.load_target = core::LoadTarget::None;
    return;
  }
  if (!state.panel_input.load_pressed) {
    return;
  }
  uint16_t input = state.panel_input.input_switches;
  uint8_t low8 = static_cast<uint8_t>(input & 0xFF);
  uint16_t full10 = static_cast<uint16_t>(input & 0x3FF);
  switch (state.panel_input.load_target) {
    case core::LoadTarget::Accumulator:
      state.accumulator.Load(low8);
      break;
    case core::LoadTarget::Buffer:
      state.buffer.Load(low8);
      break;
    case core::LoadTarget::Countdown:
      state.countdown.Load(low8);
      break;
    case core::LoadTarget::Distributor:
      state.distributor.Load(static_cast<uint16_t>(low8 & 0x0F));
      break;
    case core::LoadTarget::Opcode:
      state.opcode.Load(low8);
      break;
    case core::LoadTarget::Mar:
      state.mar.Load(full10);
      break;
    case core::LoadTarget::Par:
      state.par.Load(full10);
      break;
    case core::LoadTarget::Quotient:
      state.quotient.Load(low8);
      break;
    case core::LoadTarget::Index:
      state.index.Load(low8);
      break;
    case core::LoadTarget::None:
    default:
      break;
  }
  state.panel_input.load_pressed = false;
  state.panel_input.load_target = core::LoadTarget::None;
}

bool ApplyManualMemory(core::MachineState& state) {
  if (!state.panel_input.power_on) {
    return false;
  }
  if (!state.panel_input.start) {
    return false;
  }
  if (!state.panel_input.mem_read && !state.panel_input.mem_write) {
    return false;
  }
  uint16_t address = state.mar.value();
  if (state.panel_input.mem_write) {
    uint8_t value = static_cast<uint8_t>(state.panel_input.input_switches & 0xFF);
    state.memory.Write(address, value);
    state.mar.Load(static_cast<uint16_t>(address + 1));
  } else if (state.panel_input.mem_read) {
    uint8_t value = state.memory.Read(address);
    uint16_t upper = static_cast<uint16_t>(state.panel_input.input_switches & 0x300);
    state.panel_input.input_switches =
        static_cast<uint16_t>(upper | static_cast<uint16_t>(value));
    state.mar.Load(static_cast<uint16_t>(address + 1));
  }
  return true;
}

int ImGuiApp::Run(core::MachineState& state,
                  core::TimingEngine& timing,
                  core::ExecutionEngine& execution,
                  app::ModeController& mode,
                  const ResetHook& reset_hook) const {
  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  GLFWwindow* window = glfwCreateWindow(
      PanelLayout::kWidth, PanelLayout::kHeight, "CT-10 Emulator", nullptr,
      nullptr);
  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  PanelFonts panel_fonts = LoadPanelFonts();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL2_Init();

  DebugPane debug_pane;
  PanelView panel_view(panel_fonts.display, panel_fonts.input);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    state.panel_input.ClearMomentary();

    bool step_clock = false;
    bool step_distributor = false;

    ImVec2 display_size = ImGui::GetIO().DisplaySize;
    DrawControls(state, timing, mode, reset_hook, display_size, step_clock,
                 step_distributor);
    DrawProgramEditor(state, mode, display_size);
    panel_view.Draw(state);
    RefreshPanelStatus(state);
    ApplyPanelControls(state, timing, mode, reset_hook);
    ApplyPanelIoPreset(state);
    ApplyRegisterLoad(state);

    bool panel_step_dist = false;
    bool panel_step_phase = false;
    bool panel_step_inst = false;
    bool panel_manual_memory = false;
    if (state.panel_input.power_on) {
      if (state.panel_input.stop) {
        mode.SetMode(app::RunMode::Halted);
      }
      if (state.panel_input.start) {
        panel_manual_memory = ApplyManualMemory(state);
        if (!panel_manual_memory) {
          switch (state.panel_input.mode) {
            case 0:
              panel_step_dist = true;
              mode.SetMode(app::RunMode::Halted);
              break;
            case 1:
              panel_step_phase = true;
              mode.SetMode(app::RunMode::Halted);
              break;
            case 2:
              panel_step_inst = true;
              mode.SetMode(app::RunMode::Halted);
              break;
            case 3:
            default:
              mode.SetMode(app::RunMode::Continuous);
              break;
          }
        } else {
          mode.SetMode(app::RunMode::Halted);
        }
      }
    } else {
      mode.SetMode(app::RunMode::Halted);
    }

    state.mode.halted = mode.IsHalted();

    if (state.panel_input.power_on) {
      if (!state.mode.halted) {
        int steps = std::max(1, static_cast<int>(timing.speed_multiplier()));
        for (int i = 0; i < steps; ++i) {
          StepClock(timing, state, execution);
        }
      } else if (panel_step_inst) {
        StepInstruction(timing, state, execution);
      } else if (panel_step_phase) {
        StepUntilPhaseToggle(timing, state, execution);
      } else if (panel_step_dist) {
        StepDistributor(timing, state, execution);
      } else if (step_distributor) {
        StepDistributor(timing, state, execution);
      } else if (step_clock) {
        StepClock(timing, state, execution);
      }
    }

    if (state.mode.halted && !mode.IsHalted()) {
      mode.SetMode(app::RunMode::Halted);
    }
    state.mode.halted = mode.IsHalted();

    debug_pane.Draw(state, kDebugTop);

    ImGui::Render();

    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.08f, 0.08f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

}  // namespace ct10::ui
