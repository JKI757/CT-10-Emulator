#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "app/golden_program.h"
#include "app/program_text.h"
#include "app/tape_io.h"
#include "core/execution_engine.h"
#include "core/machine_state.h"
#include "core/timing_engine.h"

namespace {

void StepClock(ct10::core::TimingEngine& timing,
               ct10::core::MachineState& state,
               ct10::core::ExecutionEngine& execution) {
  bool was_halted = state.mode.halted;
  state.mode.halted = false;
  execution.Step(state);
  if (!state.mode.halted) {
    state.mode.halted = was_halted;
  }
  timing.Advance(state.timing);
}

int ParseMaxSteps(int argc, char** argv) {
  if (argc < 2) {
    return 200000;
  }
  char* end = nullptr;
  long value = std::strtol(argv[1], &end, 10);
  if (end == argv[1] || value <= 0) {
    return 200000;
  }
  if (value > 10000000) {
    return 10000000;
  }
  return static_cast<int>(value);
}

bool IsNumber(const char* text) {
  if (text == nullptr || text[0] == '\0') {
    return false;
  }
  char* end = nullptr;
  std::strtol(text, &end, 10);
  return end != text && *end == '\0';
}

bool LoadProgramFile(const std::string& path,
                     ct10::app::ProgramSpec& spec,
                     ct10::app::ParseResult& result,
                     std::string& error) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file) {
    error = "Unable to open program file.";
    return false;
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  const std::string content = buffer.str();
  ct10::app::ParseProgramContent(content, spec, result);
  if (spec.writes.empty()) {
    error = "No bytes parsed from program file.";
    return false;
  }
  return true;
}

bool ParseStepsValue(const char* text, int& value) {
  char* end = nullptr;
  long parsed = std::strtol(text, &end, 10);
  if (end == text || parsed <= 0) {
    return false;
  }
  if (parsed > 10000000) {
    parsed = 10000000;
  }
  value = static_cast<int>(parsed);
  return true;
}

bool LoadExpectedHexFile(const std::string& path,
                         std::vector<uint8_t>& bytes,
                         std::string& error) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file) {
    error = "Unable to open expected output file.";
    return false;
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  ct10::app::ParseResult result = ct10::app::ParseProgramText(buffer.str(), bytes);
  if (bytes.empty()) {
    error = "No bytes parsed from expected output file.";
    return false;
  }
  if (result.skipped > 0) {
    error = "Expected output file contains invalid tokens.";
    return false;
  }
  return true;
}

bool ParseIoMode(const char* text, uint8_t& mode) {
  if (std::strcmp(text, "rexmt") == 0) {
    mode = 3;
    return true;
  }
  if (std::strcmp(text, "off") == 0) {
    mode = 0;
    return true;
  }
  if (std::strcmp(text, "octal") == 0) {
    mode = 0;
    return true;
  }
  if (std::strcmp(text, "hex") == 0) {
    mode = 1;
    return true;
  }
  if (std::strcmp(text, "alpha") == 0) {
    mode = 2;
    return true;
  }
  return false;
}

bool CompareOutput(const char* label,
                   const std::vector<uint8_t>& actual,
                   const std::vector<uint8_t>& expected) {
  if (actual.size() != expected.size()) {
    std::printf("FAIL: %s size %zu (expected %zu).\n",
                label, actual.size(), expected.size());
    return false;
  }
  for (size_t i = 0; i < expected.size(); ++i) {
    if (actual[i] != expected[i]) {
      std::printf("FAIL: %s byte %zu = 0x%02X (expected 0x%02X).\n",
                  label, i,
                  static_cast<unsigned>(actual[i]),
                  static_cast<unsigned>(expected[i]));
      return false;
    }
  }
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  ct10::core::MachineState state;
  ct10::core::TimingEngine timing;
  ct10::core::ExecutionEngine execution;

  ct10::app::ProgramSpec program_spec;
  bool has_program_spec = false;
  std::string program_path;
  std::string tape_path;
  std::string terminal_in_path;
  std::string expect_term_path;
  std::string expect_printer_path;
  int max_steps = 200000;
  bool max_steps_set = false;
  bool tape_alpha = false;
  bool tape_hex = false;
  bool terminal_alpha = true;
  bool terminal_hex = false;
  bool io_mode_set = false;
  uint8_t io_mode = state.panel_input.io_mode;

  for (int i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    if (std::strcmp(arg, "--tape") == 0) {
      if (i + 1 < argc) {
        tape_path = argv[++i];
      } else {
        std::printf("FAIL: --tape requires a path.\n");
        return 3;
      }
      continue;
    }
    if (std::strcmp(arg, "--tape-alpha") == 0) {
      tape_alpha = true;
      tape_hex = false;
      continue;
    }
    if (std::strcmp(arg, "--tape-hex") == 0) {
      tape_hex = true;
      tape_alpha = false;
      continue;
    }
    if (std::strcmp(arg, "--terminal-in") == 0) {
      if (i + 1 < argc) {
        terminal_in_path = argv[++i];
      } else {
        std::printf("FAIL: --terminal-in requires a path.\n");
        return 3;
      }
      continue;
    }
    if (std::strcmp(arg, "--terminal-alpha") == 0) {
      terminal_alpha = true;
      terminal_hex = false;
      continue;
    }
    if (std::strcmp(arg, "--terminal-hex") == 0) {
      terminal_hex = true;
      terminal_alpha = false;
      continue;
    }
    if (std::strcmp(arg, "--max-steps") == 0) {
      if (i + 1 < argc) {
        if (!ParseStepsValue(argv[++i], max_steps)) {
          std::printf("FAIL: invalid --max-steps value.\n");
          return 3;
        }
        max_steps_set = true;
      } else {
        std::printf("FAIL: --max-steps requires a value.\n");
        return 3;
      }
      continue;
    }
    if (std::strcmp(arg, "--expect-term") == 0) {
      if (i + 1 < argc) {
        expect_term_path = argv[++i];
      } else {
        std::printf("FAIL: --expect-term requires a path.\n");
        return 3;
      }
      continue;
    }
    if (std::strcmp(arg, "--expect-printer") == 0) {
      if (i + 1 < argc) {
        expect_printer_path = argv[++i];
      } else {
        std::printf("FAIL: --expect-printer requires a path.\n");
        return 3;
      }
      continue;
    }
    if (std::strcmp(arg, "--io-mode") == 0) {
      if (i + 1 < argc) {
        uint8_t parsed = 1;
        if (!ParseIoMode(argv[++i], parsed)) {
          std::printf("FAIL: invalid --io-mode (rexmt|off|hex|alpha).\n");
          return 3;
        }
        io_mode = parsed;
        io_mode_set = true;
      } else {
        std::printf("FAIL: --io-mode requires a value.\n");
        return 3;
      }
      continue;
    }
    if (IsNumber(arg) && !max_steps_set) {
      if (ParseStepsValue(arg, max_steps)) {
        max_steps_set = true;
      }
      continue;
    }
    if (program_path.empty()) {
      program_path = arg;
    }
  }

  bool check_expected = program_path.empty();
  if (check_expected) {
    ct10::app::LoadGoldenProgram(state);
  } else {
    state.Reset();
    state.memory.Clear();
    ct10::app::ParseResult parse_result;
    std::string error;
    if (!LoadProgramFile(program_path, program_spec, parse_result, error)) {
      std::printf("FAIL: %s\n", error.c_str());
      return 3;
    }
    has_program_spec = true;
    for (const auto& write : program_spec.writes) {
      if (write.address >= ct10::core::Memory::kSize) {
        std::printf("FAIL: program write exceeds memory size.\n");
        return 3;
      }
      state.memory.Write(write.address, write.value);
    }
    uint16_t entry = program_spec.has_entry ? program_spec.entry : 0x000;
    state.par.Load(entry);
    check_expected = !program_spec.expects.empty();
  }

  if (!io_mode_set) {
    if (tape_alpha) {
      io_mode = 3;
    } else if (tape_hex) {
      io_mode = 2;
    } else if (!terminal_in_path.empty()) {
      io_mode = terminal_alpha ? 3 : 2;
    }
  }
  state.panel_input.io_mode = io_mode;

  if (!tape_path.empty()) {
    state.io.alpha_mode = tape_alpha;
    state.io.hex_mode = tape_hex || !tape_alpha;
    std::string error;
    if (!ct10::app::LoadTapeText(tape_path, state.io, &error)) {
      std::printf("FAIL: tape load failed: %s\n", error.c_str());
      return 3;
    }
  }

  if (!terminal_in_path.empty()) {
    ct10::core::IOState temp_io;
    temp_io.alpha_mode = terminal_alpha;
    temp_io.hex_mode = terminal_hex || !terminal_alpha;
    std::string error;
    if (!ct10::app::LoadTapeText(terminal_in_path, temp_io, &error)) {
      std::printf("FAIL: terminal input load failed: %s\n", error.c_str());
      return 3;
    }
    state.io.terminal_input = std::move(temp_io.input_data);
    state.io.terminal_input_pos = 0;
    state.io.interrupt = false;
  }

  timing.Reset(state.timing);

  int steps = 0;
  for (; steps < max_steps; ++steps) {
    StepClock(timing, state, execution);
    if (state.mode.halted) {
      break;
    }
  }

  if (!state.mode.halted) {
    std::printf("FAIL: did not halt within %d clock steps.\n", max_steps);
    std::printf("State: PAR=0x%03X OP=0x%02X MAR=0x%03X D=%u %s %s\n",
                static_cast<unsigned>(state.par.value()),
                static_cast<unsigned>(state.opcode.value()),
                static_cast<unsigned>(state.mar.value()),
                static_cast<unsigned>(state.timing.distributor),
                state.timing.phase == ct10::core::ClockPhase::CP1
                    ? "CP1"
                    : (state.timing.phase == ct10::core::ClockPhase::CP2
                           ? "CP2"
                           : "CP3"),
                state.timing.acquisition ? "acq" : "exec");
    return 2;
  }

  if (check_expected && program_path.empty()) {
    uint8_t result = state.memory.Read(ct10::app::kGoldenProgramResultAddress);
    if (result != ct10::app::kGoldenProgramExpectedValue) {
      std::printf("FAIL: memory[0x%02X] = 0x%02X (expected 0x%02X).\n",
                  ct10::app::kGoldenProgramResultAddress, result,
                  ct10::app::kGoldenProgramExpectedValue);
      return 1;
    }
    std::printf(
        "PASS: halted after %d clock steps. memory[0x%02X] = 0x%02X.\n",
        steps + 1, ct10::app::kGoldenProgramResultAddress, result);
    return 0;
  }

  if (check_expected && !program_path.empty()) {
    if (!has_program_spec) {
      std::printf("FAIL: expectations missing program specification.\n");
      return 3;
    }
    for (const auto& expect : program_spec.expects) {
      uint8_t value = state.memory.Read(expect.address);
      if (value != expect.value) {
        std::printf("FAIL: memory[0x%02X] = 0x%02X (expected 0x%02X).\n",
                    static_cast<unsigned>(expect.address),
                    static_cast<unsigned>(value),
                    static_cast<unsigned>(expect.value));
        return 1;
      }
    }
  }

  if (!expect_term_path.empty()) {
    std::vector<uint8_t> expected;
    std::string error;
    if (!LoadExpectedHexFile(expect_term_path, expected, error)) {
      std::printf("FAIL: %s\n", error.c_str());
      return 1;
    }
    if (!CompareOutput("terminal output", state.io.terminal_output, expected)) {
      return 1;
    }
  }

  if (!expect_printer_path.empty()) {
    std::vector<uint8_t> expected;
    std::string error;
    if (!LoadExpectedHexFile(expect_printer_path, expected, error)) {
      std::printf("FAIL: %s\n", error.c_str());
      return 1;
    }
    if (!CompareOutput("printer output", state.io.printer_output, expected)) {
      return 1;
    }
  }

  std::printf("PASS: halted after %d clock steps.\n", steps + 1);
  return 0;
}
