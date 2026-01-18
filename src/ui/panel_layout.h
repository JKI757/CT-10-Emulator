#pragma once

#include <array>

namespace ct10::ui {

struct Rect {
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
};

struct KeypadKey {
  char label = '0';
  Rect rect;
};

struct Lamp {
  int x = 0;
  int y = 0;
};

struct ControlSwitch {
  const char* name = "";
  Rect rect;
};

struct LampStripLayout {
  const char* label = "";
  int x = 0;
  int y = 0;
  int bits = 0;
  int gap_after = 0;
};

struct IndicatorLayout {
  const char* label = "";
  int x = 0;
  int y = 0;
};

struct ToggleLayout {
  const char* label = "";
  Rect rect;
};

struct TextLabel {
  const char* label = "";
  int x = 0;
  int y = 0;
};

struct PanelLayout {
  static constexpr int kWidth = 1280;
  static constexpr int kHeight = 720;
  static constexpr int kDisplayWidth = 720;
  static constexpr int kDisplayHeight = 480;
  static constexpr int kInputWidth = 1280;
  static constexpr int kInputHeight = 720;
  static constexpr int kLampSize = 18;
  static constexpr int kLampGap = 6;
  static constexpr int kLampGroupGap = 16;
  static constexpr int kToggleW = 36;
  static constexpr int kToggleH = 22;

  // 4x4 hex keypad (0-F) matching historical panel
  // Layout: C D E F / 8 9 A B / 4 5 6 7 / 0 1 2 3
  inline static constexpr std::array<KeypadKey, 16> kHexKeypad = {{
      {'C', {250, 200, 44, 44}},
      {'D', {300, 200, 44, 44}},
      {'E', {350, 200, 44, 44}},
      {'F', {400, 200, 44, 44}},
      {'8', {250, 250, 44, 44}},
      {'9', {300, 250, 44, 44}},
      {'A', {350, 250, 44, 44}},
      {'B', {400, 250, 44, 44}},
      {'4', {250, 300, 44, 44}},
      {'5', {300, 300, 44, 44}},
      {'6', {350, 300, 44, 44}},
      {'7', {400, 300, 44, 44}},
      {'0', {250, 350, 44, 44}},
      {'1', {300, 350, 44, 44}},
      {'2', {350, 350, 44, 44}},
      {'3', {400, 350, 44, 44}},
  }};

  // CONTROL buttons - Row 1, right side (x: 770-920)
  inline static constexpr std::array<ControlSwitch, 3> kControls = {{
      {"CLEAR", {1070, 130, 48, 26}},
      {"STOP", {1125, 130, 48, 26}},
      {"START", {1180, 130, 48, 26}},
  }};

  inline static constexpr std::array<Lamp, 8> kAccumulatorLamps = {{
      {120, 180},
      {150, 180},
      {180, 180},
      {210, 180},
      {240, 180},
      {270, 180},
      {300, 180},
      {330, 180},
  }};

  // Left side controls - labels drawn separately above
  inline static constexpr ToggleLayout kPower = {"POWER", {70, 180, 36, 60}};
  inline static constexpr ToggleLayout kLampTest = {"LAMP TEST", {55, 380, 70, 28}};
  // RESET button - top right near DIGIAC
  inline static constexpr ToggleLayout kReset = {"RESET", {760, 55, 50, 24}};

  // INPUT toggle switches - grouped as 9 8 | 7 6 5 4 | 3 2 1 0
  inline static constexpr std::array<ToggleLayout, 10> kInputToggles = {{
      {"9", {200, 55, 26, 16}},
      {"8", {232, 55, 26, 16}},
      {"7", {280, 55, 26, 16}},
      {"6", {312, 55, 26, 16}},
      {"5", {344, 55, 26, 16}},
      {"4", {376, 55, 26, 16}},
      {"3", {424, 55, 26, 16}},
      {"2", {456, 55, 26, 16}},
      {"1", {488, 55, 26, 16}},
      {"0", {520, 55, 26, 16}},
  }};

  inline static constexpr TextLabel kInputLabel = {"INPUT", 340, 30};

  // ============ RIGHT SIDE CONTROL SECTIONS ============
  // Row 1: y=120-150 - I/O MODE (left) and CONTROL (right)
  // Row 2: y=200-230 - MODE (left) and RPT/SINGLE (right)
  // Row 3: y=280-330 - ERROR BYPASS (left) and I/O READ/WRITE (right)

  // I/O MODE - Row 1, left side (x: 560-720)
  inline static constexpr std::array<ToggleLayout, 4> kIoModeToggles = {{
      {"OCTAL", {560, 130, kToggleW, kToggleH}},
      {"HEX", {605, 130, kToggleW, kToggleH}},
      {"ALPHA", {650, 130, kToggleW, kToggleH}},
      {"OFF", {695, 130, kToggleW, kToggleH}},
  }};
  inline static constexpr TextLabel kIoModeLabel = {"I/O MODE", 600, 105};

  // CONTROL - Row 1, right side (x: 770-920)
  inline static constexpr TextLabel kControlLabel = {"CONTROL", 820, 105};

  // MODE - Row 2, left side (x: 560-720)
  inline static constexpr std::array<ToggleLayout, 4> kModeToggles = {{
      {"DIST", {560, 210, kToggleW, kToggleH}},
      {"A/I", {605, 210, kToggleW, kToggleH}},
      {"INST", {650, 210, kToggleW, kToggleH}},
      {"PROG", {695, 210, kToggleW, kToggleH}},
  }};
  inline static constexpr TextLabel kModeLabel = {"MODE", 615, 185};

  // RPT and SINGLE - Row 2, right side (x: 770-860)
  inline static constexpr std::array<ToggleLayout, 2> kRptSenseToggles = {{
      {"RPT", {770, 210, kToggleW, kToggleH}},
      {"SINGLE", {820, 210, kToggleW, kToggleH}},
  }};

  // ERROR BYPASS - Row 3, left side (x: 560-680)
  inline static constexpr std::array<ToggleLayout, 3> kErrorBypassToggles = {{
      {"INST", {560, 300, kToggleW, kToggleH}},
      {"SUM", {605, 300, kToggleW, kToggleH}},
      {"OVFL", {650, 300, kToggleW, kToggleH}},
  }};
  inline static constexpr TextLabel kErrorBypassLabel = {"ERROR BYPASS", 560, 275};

  // I/O (READ/WRITE, INSTR/BLOCK) - Row 3, right side (x: 720-820)
  inline static constexpr std::array<ToggleLayout, 4> kIoToggles = {{
      {"READ", {720, 285, kToggleW, kToggleH}},
      {"WRITE", {770, 285, kToggleW, kToggleH}},
      {"INSTR", {720, 315, kToggleW, kToggleH}},
      {"BLOCK", {770, 315, kToggleW, kToggleH}},
  }};
  inline static constexpr TextLabel kIoLabel = {"I/O", 750, 260};

  // Register load buttons at bottom
  inline static constexpr std::array<ControlSwitch, 9> kRegisterLoads = {{
      {"A", {200, 420, 34, 24}},
      {"B", {240, 420, 34, 24}},
      {"C", {280, 420, 34, 24}},
      {"D", {320, 420, 34, 24}},
      {"OP", {360, 420, 40, 24}},
      {"MAR", {406, 420, 44, 24}},
      {"PAR", {456, 420, 44, 24}},
      {"Q", {506, 420, 34, 24}},
      {"X", {546, 420, 34, 24}},
  }};

  // Memory toggles
  inline static constexpr std::array<ToggleLayout, 2> kMemoryToggles = {{
      {"RD", {610, 420, kToggleW, kToggleH}},
      {"WT", {655, 420, kToggleW, kToggleH}},
  }};

  inline static constexpr TextLabel kRegisterLoadLabel = {"LOAD", 200, 395};
  inline static constexpr TextLabel kMemoryLabel = {"MEMORY", 610, 395};

  inline static constexpr std::array<LampStripLayout, 9> kLampStrips = {{
      {"ACCUMULATOR", 200, 90, 8, 4},
      {"QUOTIENT", 468, 90, 8, 4},
      {"BUFFER", 220, 170, 8, 4},
      {"INDEX", 468, 170, 8, 4},
      {"OP CODE", 240, 260, 8, 4},
      {"MEM ADDR", 468, 260, 10, 2},
      {"COUNTDOWN", 40, 420, 8, 4},
      {"DISTRIBUTOR", 282, 420, 4, 0},
      {"PROG ADDR", 412, 420, 10, 2},
  }};

  inline static constexpr std::array<IndicatorLayout, 4> kCondIndicators = {{
      {">0", 60, 60},
      {"=0", 60, 90},
      {"<0", 60, 120},
      {"CARRY", 60, 150},
  }};

  inline static constexpr TextLabel kCondLabel = {"COND CODE", 20, 40};

  inline static constexpr std::array<IndicatorLayout, 4> kStatusIndicators = {{
      {"INTER", 60, 180},
      {"SENSE", 60, 210},
      {"FLAG", 60, 240},
      {"WAIT", 60, 270},
  }};

  inline static constexpr TextLabel kStatusLabel = {"STATUS", 20, 170};

  // Error indicators matching historical panel
  inline static constexpr std::array<IndicatorLayout, 3> kErrorIndicators = {{
      {"INST", 60, 300},
      {"ADD", 60, 330},
      {"DIVIDE", 60, 360},
  }};

  inline static constexpr TextLabel kErrorLabel = {"ERROR", 20, 290};
};

}  // namespace ct10::ui
