#pragma once

#include <array>
#include <cstddef>

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
  int label_offset_x = 0;
  int label_offset_y = 0;
};

struct ControlSwitch {
  const char* name = "";
  Rect rect;
  int label_offset_x = 0;
  int label_offset_y = 0;
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
  int label_offset_x = 0;
  int label_offset_y = 0;
};

struct TextLabel {
  const char* label = "";
  int x = 0;
  int y = 0;
};

template <size_t N>
struct ToggleGroupLayout {
  TextLabel label;
  float label_offset = 18.0f;
  std::array<ToggleLayout, N> toggles;
};

template <size_t N>
struct ControlGroupLayout {
  TextLabel label;
  float label_offset = 18.0f;
  std::array<ControlSwitch, N> controls;
};

template <size_t N>
struct ButtonGroupLayout {
  TextLabel label;
  std::array<ControlSwitch, N> buttons;
};
struct HLine {
  float x1 = 0.0f;
  float x2 = 0.0f;
  float y = 0.0f;
  float thickness = 1.0f;
};

struct PanelLayout {
  // Layout tuning map (panel units):
  // - Panels/window: kRightColumnWidth, kPanelWidthMin, kPanelHeightMin, kPanelEdgeMargin.
  // - Input grid: kInputGridCol*, kInputGridRow*, k*GroupTop*.
  // - Input switches: kInputToggleBaseX, kInputToggleY, kInputBitOffsets, kInputLabelOffsetX/Y.
  // - Right groups: kIoModeGroupTop*, kControlGroupTop*, kModeGroupTop*,
  //                 kErrorBypassGroupTop*, kIoGroupTop*, kMemoryGroupTop*.
  // - Bottom row: kLoadBaseX, kLoadOffsets, kMemoryBaseX, kMemoryOffsets.
  // - Display rows: kDisplayRowY, kDisplayLeft/Right/BottomGroupBaseX.
  // - Display lines: kDisplay*Line*Offsets.
  // - Indicators: kIndicatorGroupBaseY, kIndicatorGroupLabelOffsetY.
  // - Fonts: kFontOptions, kUiFontOptionIndex, kInputFontOptionIndex, kDisplayFontOptionIndex.
  // - Labels: kPowerLabelText, kLampTestLabelLine1/2, kBrandLabelText, kResetLabelText,
  //           kDigiaLabelText.
  static constexpr int kWidth = 1280;
  static constexpr int kHeight = 720;
  static constexpr int kDisplayWidth = 720;
  static constexpr int kDisplayHeight = 480;
  static constexpr int kInputWidth = 1280;
  static constexpr int kInputHeight = 720;
  static constexpr int kButtonSize = 66;
  static constexpr int kLampSize = kButtonSize;
  static constexpr int kLampGap = 6;
  static constexpr int kLampGroupGap = 16;
  static constexpr int kToggleW = kButtonSize;
  static constexpr int kToggleH = kButtonSize;
  static constexpr float kButtonLabelOffsetY = 40.0f;
  static constexpr int kGroupLabelOffsetY = -60;
  static constexpr int kRightGroupGap = 9;
  static constexpr int kRightPairGap = 28;
  static constexpr int kRightGroupStepX = kButtonSize + kRightGroupGap;
  static constexpr int kRightPairStepX = kButtonSize + kRightPairGap;
  static constexpr int kRightLabelToToggleY = 25;
  static constexpr float kGroupLineEdgePad = -15.0f;
  static constexpr float kGroupLineLabelPad = 10.0f;
  static constexpr float kGroupLineYOffset = 15.0f;
  static constexpr float kGroupLineDropFactor = 0.5f;
  static constexpr float kGroupLineCapLength = 18.0f;
  static constexpr int kLoadGap = 6;
  static constexpr int kLoadButtonCount = 9;
  static constexpr int kLoadTotalWidth =
      kLoadButtonCount * kButtonSize + (kLoadButtonCount - 1) * kLoadGap;

  // Input panel grid (3 columns x 7 rows).
  static constexpr int kInputGridColLeftX = 70;
  static constexpr int kInputGridColMidX = 200;
  static constexpr int kInputGridColRightX = 1260;

  static constexpr int kInputGridRow0Y = 55;
  static constexpr int kInputGridRow1Y = 105;
  static constexpr int kInputGridRow2Y = 180;
  static constexpr int kInputGridRow3Y = 260;
  static constexpr int kInputGridRow4Y = 380;
  static constexpr int kInputGridRow5Y = 595;
  static constexpr int kInputGridRow6Y = 620;

  // Group anchors (top-left coordinates) derived from the grid.
  static constexpr int kPowerGroupTopX = kInputGridColLeftX;
  static constexpr int kPowerGroupTopY = kInputGridRow2Y;
  static constexpr int kBrandGroupTopX = kInputGridColLeftX;
  static constexpr int kBrandGroupTopY = kInputGridRow3Y;
  static constexpr int kLampTestGroupTopX = kInputGridColLeftX;
  static constexpr int kLampTestGroupTopY = kInputGridRow4Y;

  static constexpr int kInputGroupTopX = kInputGridColMidX;
  static constexpr int kInputGroupTopY = kInputGridRow0Y;
  static constexpr int kKeypadGroupOffsetX = 340;
  static constexpr int kKeypadGroupOffsetY = 20;
  static constexpr int kKeypadGroupTopX = kInputGridColMidX + kKeypadGroupOffsetX;
  static constexpr int kKeypadGroupTopY = kInputGridRow2Y + kKeypadGroupOffsetY;
  static constexpr int kRegisterLoadGroupTopX = kInputGridColMidX;
  static constexpr int kRegisterLoadGroupTopY = kInputGridRow6Y;

  static constexpr int kIoModeGroupTopX = kInputGridColRightX;
  static constexpr int kIoModeGroupTopY = kInputGridRow2Y;

  static constexpr int kControlGroupOffsetX = 360;
  static constexpr int kControlGroupTopX = kInputGridColRightX + kControlGroupOffsetX;
  static constexpr int kControlGroupTopY = kInputGridRow2Y;

  static constexpr int kModeGroupOffsetY = 5;
  static constexpr int kModeGroupTopX = kInputGridColRightX;
  static constexpr int kModeGroupTopY = kInputGridRow2Y + kModeGroupOffsetY;
  static constexpr int kModeRptSenseOffsetX = 400;
  static constexpr int kModeRptSenseOffsetY = 0;
  static constexpr int kErrorBypassGroupTopX = kInputGridColRightX;
  static constexpr int kErrorBypassGroupTopY = kInputGridRow5Y;

  static constexpr int kIoGroupOffsetX = kModeRptSenseOffsetX; //align the rpt/sense w/ io
  static constexpr int kIoGroupTopX = kInputGridColRightX + kIoGroupOffsetX;
  static constexpr int kIoGroupTopY = kInputGridRow5Y;
  static constexpr int kMemoryGroupTopX =
      kRegisterLoadGroupTopX + kLoadTotalWidth + kLoadGap;
  static constexpr int kMemoryGroupTopY = kInputGridRow6Y;

  struct ColorRGBA {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 255;
  };

  struct FontOption {
    const char* name = "";
    const char* path = "";
    float size = 16.0f;
    float scale = 1.0f;
  };

  static constexpr size_t kUiFontOptionIndex = 0;
  static constexpr size_t kInputFontOptionIndex = 1;
  static constexpr size_t kDisplayFontOptionIndex = 2;
  static constexpr float kFontSizeBase = 16.0f;
  static constexpr float kFontSizeScale = 0.9f;
  static constexpr float kFontSize = kFontSizeBase * kFontSizeScale;
  inline static constexpr std::array<FontOption, 12> kFontOptions = {{
      {"Default", "", kFontSize, 1.0f},
      {"TeX Gyre Heros", "assets/fonts/texgyreheros-regular.otf", kFontSize, 1.0f},
      {"TeX Gyre Heros Condensed", "assets/fonts/texgyreheroscn-regular.otf",
       kFontSize, 1.0f},
      {"TeX Gyre Adventor", "assets/fonts/texgyreadventor-regular.otf", kFontSize, 1.0f},
      {"TeX Gyre Bonum", "assets/fonts/texgyrebonum-regular.otf", kFontSize, 1.0f},
      {"TeX Gyre Cursor", "assets/fonts/texgyrecursor-regular.otf", kFontSize, 1.0f},
      {"TeX Gyre Pagella", "assets/fonts/texgyrepagella-regular.otf", kFontSize, 1.0f},
      {"TeX Gyre Schola", "assets/fonts/texgyreschola-regular.otf", kFontSize, 1.0f},
      {"TeX Gyre Termes", "assets/fonts/texgyretermes-regular.otf", kFontSize, 1.0f},
      {"Cousine", "assets/fonts/Cousine-Regular.ttf", kFontSize, 1.0f},
      {"IBM Plex Mono", "assets/fonts/IBMPlexMono-Regular.ttf", kFontSize, 1.0f},
      {"Fira Mono", "assets/fonts/FiraMono-Regular.ttf", kFontSize, 1.0f},
  }};

  static constexpr const char* kPowerLabelText = "POWER";
  static constexpr const char* kLampTestLabelLine1 = "LAMP";
  static constexpr const char* kLampTestLabelLine2 = "TEST";
  static constexpr const char* kBrandLabelText = "COM-TRAN TEN";
  static constexpr const char* kResetLabelText = "RESET";
  static constexpr const char* kDigiaLabelText = "DIGIAC corporation";
  static constexpr const char* kIoReadLabelLine1 = "READ";
  static constexpr const char* kIoReadLabelLine2 = "INTRPT";
  static constexpr const char* kIoWriteLabelLine1 = "WRITE";
  static constexpr const char* kIoWriteLabelLine2 = "BLOCK";
  static constexpr int kIoReadLabelOffsetY = -15;
  static constexpr int kIoWriteLabelOffsetY = kIoReadLabelOffsetY;

  static constexpr float kRightColumnWidth = 360.0f;
  static constexpr float kPanelWidthMin = 420.0f;
  static constexpr float kPanelWidthPadding = 30.0f;
  static constexpr float kPanelHeightMin = 520.0f;
  static constexpr float kPanelHeightPadding = 40.0f;
  static constexpr float kPanelGap = 16.0f;
  static constexpr float kTopHeightMin = 240.0f;
  static constexpr float kTopHeightRatio = 0.42f;
  static constexpr float kBottomHeightMin = 280.0f;
  static constexpr float kBottomHeightClampMin = 200.0f;
  static constexpr float kPanelEdgeMargin = 20.0f;
  static constexpr float kPanelInnerMargin = 14.0f;
  static constexpr float kPanelInnerMarginScale = 2.0f;
  static constexpr float kPanelScaleMin = 0.1f;

  static constexpr float kMomentaryButtonCornerRadius = 6.0f;
  static constexpr float kButtonCornerRadius = 4.0f;
  static constexpr float kToggleCornerRadius = 3.0f;
  static constexpr float kLampCornerRadius = 2.0f;
  static constexpr float kMomentaryButtonBorderThickness = 1.5f;
  static constexpr float kButtonBorderThickness = 1.5f;
  static constexpr float kToggleBorderThickness = 1.2f;
  static constexpr float kInputLampBorderThickness = 1.2f;
  static constexpr float kLampBorderThickness = 1.5f;
  static constexpr float kPanelBorderThickness = 2.0f;

  static constexpr float kLampStripLabelPad = 10.0f;
  static constexpr float kLampStripLabelOffset = kLampSize + kLampStripLabelPad;
  static constexpr float kLampBitLabelOffset = kLampSize;
  static constexpr float kIndicatorLabelOffsetX = kLampSize * 0.5f;
  static constexpr float kIndicatorLabelOffsetY = kLampSize * 0.1f;

  static constexpr ColorRGBA kPanelBorderColor{18, 18, 18, 255};
  static constexpr ColorRGBA kMomentaryButtonBorderColor{18, 18, 18, 255};
  static constexpr ColorRGBA kButtonBorderColor{30, 30, 30, 255};

  static constexpr ColorRGBA kTextDark{40, 35, 30, 255};
  static constexpr ColorRGBA kTextLight{230, 230, 230, 255};
  static constexpr ColorRGBA kTextMuted{200, 200, 200, 255};
  static constexpr ColorRGBA kTextIndicator{220, 220, 220, 255};

  static constexpr ColorRGBA kInputLineColor{60, 55, 50, 255};
  static constexpr ColorRGBA kDisplayLineColor{180, 180, 180, 255};

  static constexpr ColorRGBA kKeypadButtonBase{235, 230, 220, 255};
  static constexpr ColorRGBA kKeypadButtonHover{245, 240, 230, 255};
  static constexpr ColorRGBA kKeypadButtonActive{215, 210, 200, 255};
  static constexpr ColorRGBA kKeypadButtonText{30, 30, 30, 255};

  static constexpr ColorRGBA kControlButtonBase{180, 175, 165, 255};
  static constexpr ColorRGBA kControlButtonHover{200, 195, 185, 255};
  static constexpr ColorRGBA kControlButtonActive{160, 155, 145, 255};
  static constexpr ColorRGBA kControlButtonText{30, 30, 30, 255};

  static constexpr ColorRGBA kClearButtonBase{220, 200, 80, 255};
  static constexpr ColorRGBA kClearButtonHover{240, 220, 100, 255};
  static constexpr ColorRGBA kClearButtonActive{190, 170, 60, 255};
  static constexpr ColorRGBA kClearButtonText{30, 30, 30, 255};

  static constexpr ColorRGBA kStopButtonBase{200, 80, 80, 255};
  static constexpr ColorRGBA kStopButtonHover{220, 100, 100, 255};
  static constexpr ColorRGBA kStopButtonActive{170, 60, 60, 255};
  static constexpr ColorRGBA kStopButtonText{255, 255, 255, 255};

  static constexpr ColorRGBA kStartButtonBase{80, 160, 80, 255};
  static constexpr ColorRGBA kStartButtonHover{100, 180, 100, 255};
  static constexpr ColorRGBA kStartButtonActive{60, 140, 60, 255};
  static constexpr ColorRGBA kStartButtonText{255, 255, 255, 255};

  static constexpr ColorRGBA kToggleBase{140, 135, 125, 255};
  static constexpr ColorRGBA kToggleOn{160, 150, 130, 255};
  static constexpr ColorRGBA kToggleOff{80, 75, 70, 255};
  static constexpr ColorRGBA kToggleBorder{40, 40, 40, 255};
  static constexpr ColorRGBA kToggleText{30, 30, 30, 255};

  static constexpr ColorRGBA kInputLampOn{255, 196, 64, 255};
  static constexpr ColorRGBA kInputLampOff{50, 36, 22, 255};
  static constexpr ColorRGBA kInputLampBorder{26, 18, 12, 255};
  static constexpr ColorRGBA kLampOn{255, 196, 64, 255};
  static constexpr ColorRGBA kLampOff{54, 38, 22, 255};
  static constexpr ColorRGBA kLampBorder{30, 20, 10, 255};

  static constexpr ColorRGBA kInputPanelTopLeft{215, 205, 190, 255};
  static constexpr ColorRGBA kInputPanelTopRight{210, 200, 185, 255};
  static constexpr ColorRGBA kInputPanelBottomRight{195, 185, 170, 255};
  static constexpr ColorRGBA kInputPanelBottomLeft{200, 190, 175, 255};

  static constexpr ColorRGBA kDisplayPanelTopLeft{50, 55, 60, 255};
  static constexpr ColorRGBA kDisplayPanelTopRight{45, 50, 55, 255};
  static constexpr ColorRGBA kDisplayPanelBottomRight{35, 40, 45, 255};
  static constexpr ColorRGBA kDisplayPanelBottomLeft{40, 45, 50, 255};

  static constexpr int kBottomSectionLabelOffsetY = kGroupLabelOffsetY;

  static constexpr int kKeypadGroupBaseX = kKeypadGroupTopX;
  static constexpr int kKeypadGroupBaseY = kKeypadGroupTopY;
  static constexpr int kKeypadX = kKeypadGroupBaseX;
  static constexpr int kKeypadY = kKeypadGroupBaseY;
  static constexpr int kKeypadButtonSize = kButtonSize;
  static constexpr int kKeypadGap = kRightGroupGap;
  static constexpr int kKeypadStep = kKeypadButtonSize + kKeypadGap;
  static constexpr int kKeypadKey = kKeypadButtonSize;

  static constexpr int kControlGroupBaseX = kControlGroupTopX;
  static constexpr int kControlGroupBaseY = kControlGroupTopY;
  static constexpr int kControlLabelOffsetX = 50;
  static constexpr int kControlLabelOffsetY = kGroupLabelOffsetY;
  static constexpr int kControlLabelX = kControlGroupBaseX + kControlLabelOffsetX;
  static constexpr int kControlLabelY = kControlGroupBaseY + kControlLabelOffsetY;
  static constexpr int kControlButtonY = kControlGroupBaseY + kRightLabelToToggleY;
  static constexpr int kControlButtonW = kButtonSize;
  static constexpr int kControlButtonH = kButtonSize;
  static constexpr int kControlButtonBaseX = kControlGroupBaseX;
  static constexpr int kControlClearOffsetX = 0;
  static constexpr int kControlStopOffsetX = kRightGroupStepX;
  static constexpr int kControlStartOffsetX = kRightGroupStepX * 2;

  static constexpr int kPowerGroupBaseX = kPowerGroupTopX;
  static constexpr int kPowerGroupBaseY = kPowerGroupTopY;
  static constexpr int kLampTestGroupBaseX = kLampTestGroupTopX;
  static constexpr int kLampTestGroupBaseY = kLampTestGroupTopY;
  static constexpr int kBrandGroupBaseX = kBrandGroupTopX;
  static constexpr int kBrandGroupBaseY = kBrandGroupTopY;

  static constexpr int kPowerSwitchOffsetX = 0;
  static constexpr int kPowerSwitchOffsetY = 0;
  static constexpr int kLampTestSwitchOffsetX = -20;
  static constexpr int kLampTestSwitchOffsetY = 0;
  static constexpr int kBrandLabelOffsetX = 300;
  static constexpr int kBrandLabelOffsetY = 100;

  static constexpr int kPowerX = kPowerGroupBaseX + kPowerSwitchOffsetX;
  static constexpr int kPowerY = kPowerGroupBaseY + kPowerSwitchOffsetY;
  static constexpr int kPowerW = 36;
  static constexpr int kPowerH = 60;
  static constexpr int kLampTestX = kLampTestGroupBaseX + kLampTestSwitchOffsetX;
  static constexpr int kLampTestY = kLampTestGroupBaseY + kLampTestSwitchOffsetY;
  static constexpr int kLampTestW = kButtonSize;
  static constexpr int kLampTestH = kButtonSize;
  static constexpr int kPowerLabelCenterX = kPowerX + (kPowerW / 2);
  static constexpr int kPowerLabelOffsetY = -45;
  static constexpr int kPowerLabelY = kPowerY + kPowerLabelOffsetY;
  static constexpr int kLampTestLabelCenterX = kLampTestX + (kLampTestW / 2);
  static constexpr int kLampTestLabelOffsetY = -80;
  static constexpr int kLampTestLabelY = kLampTestY + kLampTestLabelOffsetY;
  static constexpr int kBrandLabelCenterX = kBrandGroupBaseX + kBrandLabelOffsetX;
  static constexpr int kBrandLabelY = kBrandGroupBaseY + kBrandLabelOffsetY;
  static constexpr int kDigiaLabelBaseX = kInputGridColRightX;
  static constexpr int kDigiaLabelBaseY = kInputGridRow1Y;
  static constexpr int kDigiaLabelOffsetX = 200;
  static constexpr int kDigiaLabelOffsetY = -40;
  static constexpr int kDigiaLabelX = kDigiaLabelBaseX + kDigiaLabelOffsetX;
  static constexpr int kDigiaLabelY = kDigiaLabelBaseY + kDigiaLabelOffsetY;

  static constexpr int kInputGroupBaseX = kInputGroupTopX;
  static constexpr int kInputGroupBaseY = kInputGroupTopY;
  static constexpr int kInputToggleBaseX = kInputGroupBaseX;
  static constexpr int kInputToggleY = kInputGroupBaseY;
  static constexpr int kInputToggleW = kButtonSize;
  static constexpr int kInputToggleH = kButtonSize;
  static constexpr int kInputBitGap = 6;
  static constexpr int kInputBitGroupGap = 16;
  static constexpr int kInputBitStep = kButtonSize + kInputBitGap;
  static constexpr int kInputGroup1Count = 2;
  static constexpr int kInputGroup2Count = 4;
  static constexpr int kInputGroup1Width =
      kInputGroup1Count * kButtonSize + (kInputGroup1Count - 1) * kInputBitGap;
  static constexpr int kInputGroup2Width =
      kInputGroup2Count * kButtonSize + (kInputGroup2Count - 1) * kInputBitGap;
  static constexpr int kInputGroup2Start = kInputGroup1Width + kInputBitGroupGap * 2;
  static constexpr int kInputGroup3Start =
      kInputGroup2Start + kInputGroup2Width + kInputBitGroupGap*4;
  static constexpr int kInputLabelOffsetX = 450;
  static constexpr int kInputLabelOffsetY = kGroupLabelOffsetY;
  static constexpr int kInputLabelX = kInputGroupBaseX + kInputLabelOffsetX;
  static constexpr int kInputLabelY = kInputGroupBaseY + kInputLabelOffsetY;
  inline static constexpr std::array<int, 10> kInputBitOffsets = {{
      0,
      kInputBitStep * 1,
      kInputGroup2Start + kInputBitStep * 0,
      kInputGroup2Start + kInputBitStep * 1,
      kInputGroup2Start + kInputBitStep * 2,
      kInputGroup2Start + kInputBitStep * 3,
      kInputGroup3Start + kInputBitStep * 0,
      kInputGroup3Start + kInputBitStep * 1,
      kInputGroup3Start + kInputBitStep * 2,
      kInputGroup3Start + kInputBitStep * 3,
  }};
  static constexpr int kResetGapX = kInputBitGroupGap * 4;
  static constexpr int kResetYOffset = 0;
  static constexpr int kResetW = kButtonSize;
  static constexpr int kResetH = kButtonSize;
  static constexpr int kResetX =
      kInputToggleBaseX + kInputBitOffsets[9] + kInputToggleW + kResetGapX;
  static constexpr int kResetY = kInputToggleY + kResetYOffset;

  static constexpr int kIoModeGroupBaseX = kIoModeGroupTopX;
  static constexpr int kIoModeGroupBaseY = kIoModeGroupTopY;
  static constexpr int kIoModeLabelOffsetX = 55;
  static constexpr int kIoModeLabelOffsetY = kGroupLabelOffsetY;
  static constexpr int kIoModeLabelX = kIoModeGroupBaseX + kIoModeLabelOffsetX;
  static constexpr int kIoModeLabelY = kIoModeGroupBaseY + kIoModeLabelOffsetY;
  static constexpr int kIoModeY = kIoModeGroupBaseY + kRightLabelToToggleY;
  static constexpr int kIoModeX0 = kIoModeGroupBaseX;
  static constexpr int kIoModeStep = kRightGroupStepX;

  static constexpr int kModeGroupBaseX = kModeGroupTopX;
  static constexpr int kModeGroupBaseY = kModeGroupTopY+200;
  static constexpr int kModeLabelOffsetX = 240;
  static constexpr int kModeLabelOffsetY = kGroupLabelOffsetY;
  static constexpr int kModeLabelX = kModeGroupBaseX + kModeLabelOffsetX;
  static constexpr int kModeLabelY = kModeGroupBaseY + kModeLabelOffsetY;
  static constexpr int kModeY = kModeGroupBaseY + kRightLabelToToggleY;
  static constexpr int kModeX0 = kModeGroupBaseX;
  static constexpr int kModeStep = kRightGroupStepX;

  static constexpr int kModeRptSenseBaseX = kModeGroupBaseX + kModeRptSenseOffsetX;
  static constexpr int kModeRptSenseBaseY =
      kModeGroupBaseY + kRightLabelToToggleY + kModeRptSenseOffsetY;
  static constexpr int kModeRptLabelOffsetX = -12;
  static constexpr int kModeSingleLabelOffsetX = -20;

  static constexpr int kIoGroupBaseX = kIoGroupTopX;
  static constexpr int kIoGroupBaseY = kIoGroupTopY;
  static constexpr int kIoLabelOffsetX = 60;
  static constexpr int kIoLabelOffsetY = kGroupLabelOffsetY*1.2;
  static constexpr int kIoLabelY = kIoGroupBaseY + kIoLabelOffsetY;
  static constexpr int kIoLabelX = kIoGroupBaseX + kIoLabelOffsetX;
  static constexpr int kIoColBaseX = kIoGroupBaseX;
  static constexpr int kIoColOffsetX = kRightPairStepX;
  static constexpr int kIoRowBaseY = kIoGroupBaseY + kRightLabelToToggleY;

  static constexpr int kLoadLabelOffsetX = 0;
  static constexpr int kRegisterLoadGroupBaseX = kRegisterLoadGroupTopX;
  static constexpr int kRegisterLoadGroupBaseY = kRegisterLoadGroupTopY;
  static constexpr int kLoadLabelY =
      kRegisterLoadGroupBaseY + kBottomSectionLabelOffsetY;
  static constexpr int kLoadY = kRegisterLoadGroupBaseY;
  static constexpr int kLoadBaseX = kRegisterLoadGroupBaseX;
  static constexpr int kLoadLabelX = kLoadBaseX + kLoadLabelOffsetX;
  static constexpr int kLoadButtonH = kButtonSize;
  static constexpr int kLoadStep = kButtonSize + kLoadGap;
  inline static constexpr std::array<int, 9> kLoadOffsets = {{
      kLoadStep * 0,
      kLoadStep * 1,
      kLoadStep * 2,
      kLoadStep * 3,
      kLoadStep * 4,
      kLoadStep * 5,
      kLoadStep * 6,
      kLoadStep * 7,
      kLoadStep * 8,
  }};
  inline static constexpr std::array<int, 9> kLoadWidths = {{
      kButtonSize, kButtonSize, kButtonSize, kButtonSize, kButtonSize,
      kButtonSize, kButtonSize, kButtonSize, kButtonSize
  }};

  static constexpr int kMemoryLabelOffsetX = 15;
  static constexpr int kMemoryGroupBaseX = kMemoryGroupTopX;
  static constexpr int kMemoryGroupBaseY = kMemoryGroupTopY;
  static constexpr int kMemoryLabelY =
      kMemoryGroupBaseY + kBottomSectionLabelOffsetY-10;
  static constexpr int kMemoryY = kMemoryGroupBaseY;
  static constexpr int kMemoryBaseX = kMemoryGroupBaseX;
  static constexpr int kMemoryLabelX = kMemoryBaseX + kMemoryLabelOffsetX;
  static constexpr int kMemoryGap = 6;
  inline static constexpr std::array<int, 2> kMemoryOffsets = {{
      0,
      kButtonSize + kMemoryGap
  }};

  static constexpr int kErrorBypassGroupBaseX = kErrorBypassGroupTopX;
  static constexpr int kErrorBypassGroupBaseY = kErrorBypassGroupTopY;
  static constexpr int kErrorBypassLabelOffsetX = 15;
  static constexpr int kErrorBypassLabelOffsetY = kGroupLabelOffsetY * 1.2;
  static constexpr int kErrorBypassLabelX =
      kErrorBypassGroupBaseX + kErrorBypassLabelOffsetX;
  static constexpr int kErrorBypassLabelY =
      kErrorBypassGroupBaseY + kErrorBypassLabelOffsetY;
  static constexpr int kErrorBypassY =
      kErrorBypassGroupBaseY + kRightLabelToToggleY;
  static constexpr int kErrorBypassX0 = kErrorBypassGroupBaseX;
  static constexpr int kErrorBypassStep = kRightGroupStepX;

  static constexpr int kDisplayRowTopIndex = 0;
  static constexpr int kDisplayRowMidIndex = 1;
  static constexpr int kDisplayRowOpIndex = 2;
  static constexpr int kDisplayRowBottomIndex = 3;
  inline static constexpr std::array<int, 4> kDisplayRowY = {{90, 170, 260, 420}};

  static constexpr int kDisplayLeftGroupBaseX = 200;
  static constexpr int kDisplayRightGroupBaseX = 468;
  static constexpr int kDisplayBottomGroupBaseX = 40;
  static constexpr int kDisplayLeftBaseX = kDisplayLeftGroupBaseX;
  inline static constexpr std::array<int, 3> kDisplayLeftRowOffsets = {{0, 20, 40}};

  static constexpr int kDisplayRightBaseX = kDisplayRightGroupBaseX;
  inline static constexpr std::array<int, 3> kDisplayRightRowOffsets = {{0, 0, 0}};

  static constexpr int kDisplayBottomBaseX = kDisplayBottomGroupBaseX;
  inline static constexpr std::array<int, 3> kDisplayBottomOffsets = {{0, 242, 372}};
  static constexpr int kLampBits8 = 8;
  static constexpr int kLampBits10 = 10;
  static constexpr int kLampBits4 = 4;
  static constexpr int kLampGap4 = 4;
  static constexpr int kLampGap2 = 2;
  static constexpr int kLampGap0 = 0;

  static constexpr int kAccumulatorGroupBaseX =
      kDisplayLeftGroupBaseX + kDisplayLeftRowOffsets[kDisplayRowTopIndex];
  static constexpr int kAccumulatorGroupBaseY = kDisplayRowY[kDisplayRowTopIndex];
  static constexpr int kQuotientGroupBaseX =
      kDisplayRightGroupBaseX + kDisplayRightRowOffsets[kDisplayRowTopIndex];
  static constexpr int kQuotientGroupBaseY = kDisplayRowY[kDisplayRowTopIndex];
  static constexpr int kBufferGroupBaseX =
      kDisplayLeftGroupBaseX + kDisplayLeftRowOffsets[kDisplayRowMidIndex];
  static constexpr int kBufferGroupBaseY = kDisplayRowY[kDisplayRowMidIndex];
  static constexpr int kIndexGroupBaseX =
      kDisplayRightGroupBaseX + kDisplayRightRowOffsets[kDisplayRowMidIndex];
  static constexpr int kIndexGroupBaseY = kDisplayRowY[kDisplayRowMidIndex];
  static constexpr int kOpCodeGroupBaseX =
      kDisplayLeftGroupBaseX + kDisplayLeftRowOffsets[kDisplayRowOpIndex];
  static constexpr int kOpCodeGroupBaseY = kDisplayRowY[kDisplayRowOpIndex];
  static constexpr int kMemAddrGroupBaseX =
      kDisplayRightGroupBaseX + kDisplayRightRowOffsets[kDisplayRowOpIndex];
  static constexpr int kMemAddrGroupBaseY = kDisplayRowY[kDisplayRowOpIndex];
  static constexpr int kCountdownGroupBaseX =
      kDisplayBottomGroupBaseX + kDisplayBottomOffsets[0];
  static constexpr int kCountdownGroupBaseY = kDisplayRowY[kDisplayRowBottomIndex];
  static constexpr int kDistributorGroupBaseX =
      kDisplayBottomGroupBaseX + kDisplayBottomOffsets[1];
  static constexpr int kDistributorGroupBaseY = kDisplayRowY[kDisplayRowBottomIndex];
  static constexpr int kProgramAddressGroupBaseX =
      kDisplayBottomGroupBaseX + kDisplayBottomOffsets[2];
  static constexpr int kProgramAddressGroupBaseY = kDisplayRowY[kDisplayRowBottomIndex];

  static constexpr int kIndicatorGroupBaseXDefault = 60;
  static constexpr int kIndicatorGroupLabelOffsetX = -40;
  static constexpr int kIndicatorStepY = 30;
  static constexpr int kIndicatorCondIndex = 0;
  static constexpr int kIndicatorStatusIndex = 1;
  static constexpr int kIndicatorErrorIndex = 2;
  static constexpr int kCondGroupBaseX = kIndicatorGroupBaseXDefault;
  static constexpr int kCondGroupBaseY = 60;
  static constexpr int kStatusGroupBaseX = kIndicatorGroupBaseXDefault;
  static constexpr int kStatusGroupBaseY = 180;
  static constexpr int kErrorGroupBaseX = kIndicatorGroupBaseXDefault;
  static constexpr int kErrorGroupBaseY = 300;
  inline static constexpr std::array<int, 3> kIndicatorGroupBaseX = {{
      kCondGroupBaseX, kStatusGroupBaseX, kErrorGroupBaseX,
  }};
  inline static constexpr std::array<int, 3> kIndicatorGroupBaseY = {{
      kCondGroupBaseY, kStatusGroupBaseY, kErrorGroupBaseY,
  }};
  inline static constexpr std::array<int, 3> kIndicatorGroupLabelOffsetY = {{
      kGroupLabelOffsetY, kGroupLabelOffsetY, kGroupLabelOffsetY
  }};


  static constexpr float kInputLineThickness = 1.5f;
  static constexpr float kSectionLineThickness = 1.5f;
  static constexpr float kDisplayLineThickness = 1.0f;

  static constexpr int kPowerLabelX = kPowerLabelCenterX;
  static constexpr int kLampTestLabelX = kLampTestLabelCenterX;
  static constexpr int kBrandLabelX = kBrandLabelCenterX;
  static constexpr float kResetLabelOffsetX = 0.0f;
  static constexpr float kResetLabelOffsetY = 40.0f;

  static constexpr float kDisplayBottomMargin = 40.0f;
  static constexpr float kDisplayBottomMinSpacing = 10.0f;
  static constexpr float kDisplayBottomSpacingSlots = 4.0f;
  static constexpr int kTopLampStripCount = 6;
  static constexpr int kCountdownStripIndex = 6;
  static constexpr int kDistributorStripIndex = 7;
  static constexpr int kProgramAddressStripIndex = 8;

  inline static constexpr std::array<float, 3> kDisplayLineYOffsets = {{
      10.0f, 10.0f, 10.0f
  }};
  inline static constexpr std::array<float, 3> kDisplayLeftLineStartOffsets = {{
      -70.0f, -90.0f, -110.0f
  }};
  inline static constexpr std::array<float, 3> kDisplayLeftLineEndOffsets = {{
      -5.0f, -5.0f, -5.0f
  }};
  inline static constexpr std::array<float, 3> kDisplayRightLineStartAOffsets = {{
      -98.0f, -98.0f, -68.0f
  }};
  inline static constexpr std::array<float, 3> kDisplayRightLineEndAOffsets = {{
      -48.0f, -48.0f, -48.0f
  }};
  inline static constexpr std::array<float, 3> kDisplayRightLineStartBOffsets = {{
      -3.0f, -3.0f, -3.0f
  }};
  inline static constexpr std::array<float, 3> kDisplayRightLineEndBOffsets = {{
      62.0f, 62.0f, 82.0f
  }};

  static constexpr float kDisplayLineTopY =
      kDisplayRowY[kDisplayRowTopIndex] + kDisplayLineYOffsets[kDisplayRowTopIndex];
  static constexpr float kDisplayLineMidY =
      kDisplayRowY[kDisplayRowMidIndex] + kDisplayLineYOffsets[kDisplayRowMidIndex];
  static constexpr float kDisplayLineOpY =
      kDisplayRowY[kDisplayRowOpIndex] + kDisplayLineYOffsets[kDisplayRowOpIndex];

  static constexpr float kDisplayLineTopX1 =
      kDisplayLeftBaseX + kDisplayLeftRowOffsets[kDisplayRowTopIndex] +
      kDisplayLeftLineStartOffsets[kDisplayRowTopIndex];
  static constexpr float kDisplayLineTopX2 =
      kDisplayLeftBaseX + kDisplayLeftRowOffsets[kDisplayRowTopIndex] +
      kDisplayLeftLineEndOffsets[kDisplayRowTopIndex];
  static constexpr float kDisplayLineTopX3 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowTopIndex] +
      kDisplayRightLineStartAOffsets[kDisplayRowTopIndex];
  static constexpr float kDisplayLineTopX4 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowTopIndex] +
      kDisplayRightLineEndAOffsets[kDisplayRowTopIndex];
  static constexpr float kDisplayLineTopX5 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowTopIndex] +
      kDisplayRightLineStartBOffsets[kDisplayRowTopIndex];
  static constexpr float kDisplayLineTopX6 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowTopIndex] +
      kDisplayRightLineEndBOffsets[kDisplayRowTopIndex];

  static constexpr float kDisplayLineMidX1 =
      kDisplayLeftBaseX + kDisplayLeftRowOffsets[kDisplayRowMidIndex] +
      kDisplayLeftLineStartOffsets[kDisplayRowMidIndex];
  static constexpr float kDisplayLineMidX2 =
      kDisplayLeftBaseX + kDisplayLeftRowOffsets[kDisplayRowMidIndex] +
      kDisplayLeftLineEndOffsets[kDisplayRowMidIndex];
  static constexpr float kDisplayLineMidX3 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowMidIndex] +
      kDisplayRightLineStartAOffsets[kDisplayRowMidIndex];
  static constexpr float kDisplayLineMidX4 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowMidIndex] +
      kDisplayRightLineEndAOffsets[kDisplayRowMidIndex];
  static constexpr float kDisplayLineMidX5 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowMidIndex] +
      kDisplayRightLineStartBOffsets[kDisplayRowMidIndex];
  static constexpr float kDisplayLineMidX6 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowMidIndex] +
      kDisplayRightLineEndBOffsets[kDisplayRowMidIndex];

  static constexpr float kDisplayLineOpX1 =
      kDisplayLeftBaseX + kDisplayLeftRowOffsets[kDisplayRowOpIndex] +
      kDisplayLeftLineStartOffsets[kDisplayRowOpIndex];
  static constexpr float kDisplayLineOpX2 =
      kDisplayLeftBaseX + kDisplayLeftRowOffsets[kDisplayRowOpIndex] +
      kDisplayLeftLineEndOffsets[kDisplayRowOpIndex];
  static constexpr float kDisplayLineOpX3 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowOpIndex] +
      kDisplayRightLineStartAOffsets[kDisplayRowOpIndex];
  static constexpr float kDisplayLineOpX4 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowOpIndex] +
      kDisplayRightLineEndAOffsets[kDisplayRowOpIndex];
  static constexpr float kDisplayLineOpX5 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowOpIndex] +
      kDisplayRightLineStartBOffsets[kDisplayRowOpIndex];
  static constexpr float kDisplayLineOpX6 =
      kDisplayRightBaseX + kDisplayRightRowOffsets[kDisplayRowOpIndex] +
      kDisplayRightLineEndBOffsets[kDisplayRowOpIndex];

  inline static constexpr std::array<float, 3> kDisplayLineThicknesses = {{
      kDisplayLineThickness, kDisplayLineThickness, kDisplayLineThickness
  }};
  inline static constexpr std::array<HLine, 9> kDisplayPanelLines = {{
      {kDisplayLineTopX1, kDisplayLineTopX2, kDisplayLineTopY,
       kDisplayLineThicknesses[kDisplayRowTopIndex]},
      {kDisplayLineTopX3, kDisplayLineTopX4, kDisplayLineTopY,
       kDisplayLineThicknesses[kDisplayRowTopIndex]},
      {kDisplayLineTopX5, kDisplayLineTopX6, kDisplayLineTopY,
       kDisplayLineThicknesses[kDisplayRowTopIndex]},
      {kDisplayLineMidX1, kDisplayLineMidX2, kDisplayLineMidY,
       kDisplayLineThicknesses[kDisplayRowMidIndex]},
      {kDisplayLineMidX3, kDisplayLineMidX4, kDisplayLineMidY,
       kDisplayLineThicknesses[kDisplayRowMidIndex]},
      {kDisplayLineMidX5, kDisplayLineMidX6, kDisplayLineMidY,
       kDisplayLineThicknesses[kDisplayRowMidIndex]},
      {kDisplayLineOpX1, kDisplayLineOpX2, kDisplayLineOpY,
       kDisplayLineThicknesses[kDisplayRowOpIndex]},
      {kDisplayLineOpX3, kDisplayLineOpX4, kDisplayLineOpY,
       kDisplayLineThicknesses[kDisplayRowOpIndex]},
      {kDisplayLineOpX5, kDisplayLineOpX6, kDisplayLineOpY,
       kDisplayLineThicknesses[kDisplayRowOpIndex]},
  }};

  // 4x4 hex keypad (0-F) matching historical panel
  // Layout: C D E F / 8 9 A B / 4 5 6 7 / 0 1 2 3
  inline static constexpr std::array<KeypadKey, 16> kHexKeypad = {{
      {'C', {kKeypadX + 0 * kKeypadStep, kKeypadY + 0 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'D', {kKeypadX + 1 * kKeypadStep, kKeypadY + 0 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'E', {kKeypadX + 2 * kKeypadStep, kKeypadY + 0 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'F', {kKeypadX + 3 * kKeypadStep, kKeypadY + 0 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'8', {kKeypadX + 0 * kKeypadStep, kKeypadY + 1 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'9', {kKeypadX + 1 * kKeypadStep, kKeypadY + 1 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'A', {kKeypadX + 2 * kKeypadStep, kKeypadY + 1 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'B', {kKeypadX + 3 * kKeypadStep, kKeypadY + 1 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'4', {kKeypadX + 0 * kKeypadStep, kKeypadY + 2 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'5', {kKeypadX + 1 * kKeypadStep, kKeypadY + 2 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'6', {kKeypadX + 2 * kKeypadStep, kKeypadY + 2 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'7', {kKeypadX + 3 * kKeypadStep, kKeypadY + 2 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'0', {kKeypadX + 0 * kKeypadStep, kKeypadY + 3 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'1', {kKeypadX + 1 * kKeypadStep, kKeypadY + 3 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'2', {kKeypadX + 2 * kKeypadStep, kKeypadY + 3 * kKeypadStep, kKeypadKey, kKeypadKey}},
      {'3', {kKeypadX + 3 * kKeypadStep, kKeypadY + 3 * kKeypadStep, kKeypadKey, kKeypadKey}},
  }};

  // CONTROL buttons - Row 1, right side (x: 770-920)
  inline static constexpr ControlGroupLayout<3> kControlGroup = {
      {"CONTROL", kControlLabelX, kControlLabelY},
      kButtonLabelOffsetY,
      {{
          {"CLEAR", {kControlButtonBaseX + kControlClearOffsetX,
                     kControlButtonY, kControlButtonW, kControlButtonH}},
          {"STOP", {kControlButtonBaseX + kControlStopOffsetX,
                    kControlButtonY, kControlButtonW, kControlButtonH}},
          {"START", {kControlButtonBaseX + kControlStartOffsetX,
                     kControlButtonY, kControlButtonW, kControlButtonH}},
      }},
  };

  static constexpr float kControlGroupLeftX = kControlButtonBaseX;
  static constexpr float kControlGroupRightX =
      kControlButtonBaseX + kControlStartOffsetX + kControlButtonW;
  static constexpr float kControlLineY = kControlLabelY + kGroupLineYOffset;
  static constexpr float kControlLineBottomY =
      kControlButtonY + kControlButtonH * kGroupLineDropFactor;

  // Left side controls - labels drawn separately above
  inline static constexpr ToggleLayout kPower = {"POWER",
                                                 {kPowerX, kPowerY, kPowerW, kPowerH}};
  inline static constexpr ToggleLayout kLampTest = {"LAMP TEST",
                                                    {kLampTestX, kLampTestY,
                                                     kLampTestW, kLampTestH}};
  // RESET button - top right near DIGIAC
  inline static constexpr ToggleLayout kReset = {"RESET",
                                                 {kResetX, kResetY, kResetW, kResetH}};

  // INPUT toggle switches - grouped as 9 8 | 7 6 5 4 | 3 2 1 0
  inline static constexpr ToggleGroupLayout<10> kInputGroup = {
      {"INPUT", kInputLabelX, kInputLabelY},
      kButtonLabelOffsetY,
      {{
          {"9", {kInputToggleBaseX + kInputBitOffsets[0], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"8", {kInputToggleBaseX + kInputBitOffsets[1], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"7", {kInputToggleBaseX + kInputBitOffsets[2], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"6", {kInputToggleBaseX + kInputBitOffsets[3], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"5", {kInputToggleBaseX + kInputBitOffsets[4], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"4", {kInputToggleBaseX + kInputBitOffsets[5], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"3", {kInputToggleBaseX + kInputBitOffsets[6], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"2", {kInputToggleBaseX + kInputBitOffsets[7], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"1", {kInputToggleBaseX + kInputBitOffsets[8], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
          {"0", {kInputToggleBaseX + kInputBitOffsets[9], kInputToggleY,
                 kInputToggleW, kInputToggleH}},
      }},
  };

  static constexpr float kInputHeaderLineYOffset = 15.0f;
  static constexpr float kInputHeaderLineLeftStartOffsetX = -480.0f;
  static constexpr float kInputHeaderLineLeftEndOffsetX = -20.0f;
  static constexpr float kInputHeaderLineRightStartOffsetX = 90.0f;
  static constexpr float kInputHeaderLineRightEndOffsetX = 510.0f;
  static constexpr float kInputHeaderLineY = kInputLabelY + kInputHeaderLineYOffset;
  static constexpr float kInputHeaderLineLeftX1 =
      kInputLabelX + kInputHeaderLineLeftStartOffsetX;
  static constexpr float kInputHeaderLineLeftX2 =
      kInputLabelX + kInputHeaderLineLeftEndOffsetX;
  static constexpr float kInputHeaderLineRightX1 =
      kInputLabelX + kInputHeaderLineRightStartOffsetX;
  static constexpr float kInputHeaderLineRightX2 =
      kInputLabelX + kInputHeaderLineRightEndOffsetX;
  inline static constexpr std::array<HLine, 2> kInputHeaderLines = {{
      {kInputHeaderLineLeftX1, kInputHeaderLineLeftX2,
       kInputHeaderLineY, kInputLineThickness},
      {kInputHeaderLineRightX1, kInputHeaderLineRightX2,
       kInputHeaderLineY, kInputLineThickness},
  }};

  // ============ RIGHT SIDE CONTROL SECTIONS ============
  // Row 1: I/O MODE (left) and CONTROL (right)
  // Row 2: MODE (includes RPT/SINGLE)
  // Row 5: ERROR BYPASS (left) and I/O READ/WRITE (right)

  // I/O MODE - Row 1, left side
  inline static constexpr ToggleGroupLayout<3> kIoModeGroup = {
      {"I/O MODE", kIoModeLabelX, kIoModeLabelY},
      kButtonLabelOffsetY,
      {{
          {"OCTAL", {kIoModeX0 + 0 * kIoModeStep, kIoModeY, kToggleW, kToggleH}, .label_offset_x = -10},
          {"HEX", {kIoModeX0 + 1 * kIoModeStep, kIoModeY, kToggleW, kToggleH}, .label_offset_x = -5},
          {"ALPHA", {kIoModeX0 + 2 * kIoModeStep, kIoModeY, kToggleW, kToggleH}, .label_offset_x = -10},
      }},
  };

  static constexpr float kIoModeGroupLeftX = kIoModeX0;
  static constexpr float kIoModeGroupRightX = kIoModeX0 + kIoModeStep * 2 + kToggleW;
  static constexpr float kIoModeLineY = kIoModeLabelY + kGroupLineYOffset;
  static constexpr float kIoModeLineBottomY =
      kIoModeY + kToggleH * kGroupLineDropFactor;

  // MODE - Row 2 (includes RPT/SINGLE).
  inline static constexpr ToggleGroupLayout<6> kModeGroup = {
      {"MODE", kModeLabelX, kModeLabelY},
      kButtonLabelOffsetY,
      {{
          {"DIST", {kModeX0 + 0 * kModeStep, kModeY, kToggleW, kToggleH}},
          {"A/E", {kModeX0 + 1 * kModeStep, kModeY, kToggleW, kToggleH}},
          {"INST", {kModeX0 + 2 * kModeStep, kModeY, kToggleW, kToggleH}},
          {"PROG", {kModeX0 + 3 * kModeStep, kModeY, kToggleW, kToggleH}},
          {"RPT", {kModeRptSenseBaseX + 0 * kRightPairStepX,
                   kModeRptSenseBaseY, kToggleW, kToggleH},
                   kModeRptLabelOffsetX},
          {"SINGLE", {kModeRptSenseBaseX + 1 * kRightPairStepX,
                      kModeRptSenseBaseY, kToggleW, kToggleH},
                      kModeSingleLabelOffsetX},
      }},
  };

  static constexpr float kModeGroupLeftX =
      (kModeRptSenseBaseX < kModeX0) ? kModeRptSenseBaseX : kModeX0;
  static constexpr float kModeGroupRightX =
      (kModeRptSenseBaseX + kRightPairStepX + kToggleW >
       kModeX0 + kModeStep * 3 + kToggleW)
          ? (kModeRptSenseBaseX + kRightPairStepX + kToggleW)
          : (kModeX0 + kModeStep * 3 + kToggleW);
  static constexpr float kModeLineY = kModeLabelY + kGroupLineYOffset;
  static constexpr float kModeLineBottomY =
      ((kModeRptSenseBaseY > kModeY) ? kModeRptSenseBaseY : kModeY) +
      kToggleH * kGroupLineDropFactor;

  // ERROR BYPASS - Row 5, left side
  inline static constexpr ToggleGroupLayout<3> kErrorBypassGroup = {
      {"ERROR BYPASS", kErrorBypassLabelX, kErrorBypassLabelY},
      kButtonLabelOffsetY,
      {{
          {"INST", {kErrorBypassX0 + 0 * kErrorBypassStep, kErrorBypassY,
                    kToggleW, kToggleH}, .label_offset_x = -5},
          {"ADD\nOVFL", {kErrorBypassX0 + 1 * kErrorBypassStep, kErrorBypassY,
                   kToggleW, kToggleH}, .label_offset_x = -5, .label_offset_y = -30},
          {"DIV\nOVFL", {kErrorBypassX0 + 2 * kErrorBypassStep, kErrorBypassY,
                    kToggleW, kToggleH}, .label_offset_x = -5, .label_offset_y = -30},
      }},
  };

  static constexpr float kErrorBypassGroupLeftX = kErrorBypassX0;
  static constexpr float kErrorBypassGroupRightX =
      kErrorBypassX0 + kErrorBypassStep * 2 + kToggleW;
  static constexpr float kErrorBypassLineY = kErrorBypassLabelY + kGroupLineYOffset;
  static constexpr float kErrorBypassLineBottomY =
      kErrorBypassY + kToggleH * kGroupLineDropFactor;

  // I/O (READ/INTRPT, WRITE/BLOCK) - Row 5, right side
  inline static constexpr ToggleGroupLayout<2> kIoGroup = {
      {"I/O", kIoLabelX, kIoLabelY},
      kButtonLabelOffsetY,
      {{
          {"READ", {kIoColBaseX + 0 * kIoColOffsetX, kIoRowBaseY,
                    kToggleW, kToggleH}},
          {"WRITE", {kIoColBaseX + 1 * kIoColOffsetX, kIoRowBaseY,
                     kToggleW, kToggleH}},
      }},
  };

  static constexpr float kIoGroupLeftX = kIoColBaseX;
  static constexpr float kIoGroupRightX = kIoColBaseX + kIoColOffsetX + kToggleW;
  static constexpr float kIoLineY = kIoLabelY + kGroupLineYOffset;
  static constexpr float kIoLineBottomY =
      kIoRowBaseY + kToggleH * kGroupLineDropFactor;

  // Register load buttons at bottom
  inline static constexpr ButtonGroupLayout<9> kRegisterLoadGroup = {
      {"LOAD", kLoadLabelX, kLoadLabelY},
      {{
          {"A", {kLoadBaseX + kLoadOffsets[0], kLoadY, kLoadWidths[0], kLoadButtonH}},
          {"B", {kLoadBaseX + kLoadOffsets[1], kLoadY, kLoadWidths[1], kLoadButtonH}},
          {"C", {kLoadBaseX + kLoadOffsets[2], kLoadY, kLoadWidths[2], kLoadButtonH}},
          {"D", {kLoadBaseX + kLoadOffsets[3], kLoadY, kLoadWidths[3], kLoadButtonH}},
          {"OP", {kLoadBaseX + kLoadOffsets[4], kLoadY, kLoadWidths[4], kLoadButtonH}},
          {"MAR", {kLoadBaseX + kLoadOffsets[5], kLoadY, kLoadWidths[5], kLoadButtonH}},
          {"PAR", {kLoadBaseX + kLoadOffsets[6], kLoadY, kLoadWidths[6], kLoadButtonH}},
          {"Q", {kLoadBaseX + kLoadOffsets[7], kLoadY, kLoadWidths[7], kLoadButtonH}},
          {"X", {kLoadBaseX + kLoadOffsets[8], kLoadY, kLoadWidths[8], kLoadButtonH}},
      }},
  };

  // Memory toggles
  inline static constexpr ToggleGroupLayout<2> kMemoryGroup = {
      {"MEMORY", kMemoryLabelX, kMemoryLabelY},
      kButtonLabelOffsetY,
      {{
          {"RD", {kMemoryBaseX + kMemoryOffsets[0], kMemoryY, kToggleW, kToggleH}, .label_offset_x = -5},
          {"WT", {kMemoryBaseX + kMemoryOffsets[1], kMemoryY, kToggleW, kToggleH}, .label_offset_x = -5},
      }},
  };

  inline static constexpr std::array<LampStripLayout, 9> kLampStrips = {{
      {"ACCUMULATOR",
       kAccumulatorGroupBaseX, kAccumulatorGroupBaseY, kLampBits8, kLampGap4},
      {"QUOTIENT",
       kQuotientGroupBaseX, kQuotientGroupBaseY, kLampBits8, kLampGap4},
      {"BUFFER",
       kBufferGroupBaseX, kBufferGroupBaseY, kLampBits8, kLampGap4},
      {"INDEX",
       kIndexGroupBaseX, kIndexGroupBaseY, kLampBits8, kLampGap4},
      {"OP CODE",
       kOpCodeGroupBaseX, kOpCodeGroupBaseY, kLampBits8, kLampGap4},
      {"MEM ADDR",
       kMemAddrGroupBaseX, kMemAddrGroupBaseY, kLampBits10, kLampGap2},
      {"COUNTDOWN",
       kCountdownGroupBaseX, kCountdownGroupBaseY, kLampBits8, kLampGap4},
      {"DISTRIBUTOR",
       kDistributorGroupBaseX, kDistributorGroupBaseY, kLampBits4, kLampGap0},
      {"PROG ADDR",
       kProgramAddressGroupBaseX, kProgramAddressGroupBaseY, kLampBits10, kLampGap2},
  }};

  inline static constexpr std::array<IndicatorLayout, 4> kCondIndicators = {{
      {">0",
       kIndicatorGroupBaseX[kIndicatorCondIndex],
       kIndicatorGroupBaseY[kIndicatorCondIndex] + 0 * kIndicatorStepY},
      {"=0",
       kIndicatorGroupBaseX[kIndicatorCondIndex],
       kIndicatorGroupBaseY[kIndicatorCondIndex] + 1 * kIndicatorStepY},
      {"<0",
       kIndicatorGroupBaseX[kIndicatorCondIndex],
       kIndicatorGroupBaseY[kIndicatorCondIndex] + 2 * kIndicatorStepY},
      {"CARRY",
       kIndicatorGroupBaseX[kIndicatorCondIndex],
       kIndicatorGroupBaseY[kIndicatorCondIndex] + 3 * kIndicatorStepY},
  }};

  inline static constexpr TextLabel kCondLabel = {
      "COND CODE",
      kIndicatorGroupBaseX[kIndicatorCondIndex] + kIndicatorGroupLabelOffsetX,
      kIndicatorGroupBaseY[kIndicatorCondIndex] +
          kIndicatorGroupLabelOffsetY[kIndicatorCondIndex],
  };

  inline static constexpr std::array<IndicatorLayout, 4> kStatusIndicators = {{
      {"INTER",
       kIndicatorGroupBaseX[kIndicatorStatusIndex],
       kIndicatorGroupBaseY[kIndicatorStatusIndex] + 0 * kIndicatorStepY},
      {"SENSE",
       kIndicatorGroupBaseX[kIndicatorStatusIndex],
       kIndicatorGroupBaseY[kIndicatorStatusIndex] + 1 * kIndicatorStepY},
      {"FLAG",
       kIndicatorGroupBaseX[kIndicatorStatusIndex],
       kIndicatorGroupBaseY[kIndicatorStatusIndex] + 2 * kIndicatorStepY},
      {"WAIT",
       kIndicatorGroupBaseX[kIndicatorStatusIndex],
       kIndicatorGroupBaseY[kIndicatorStatusIndex] + 3 * kIndicatorStepY},
  }};

  inline static constexpr TextLabel kStatusLabel = {
      "STATUS",
      kIndicatorGroupBaseX[kIndicatorStatusIndex] + kIndicatorGroupLabelOffsetX,
      kIndicatorGroupBaseY[kIndicatorStatusIndex] +
          kIndicatorGroupLabelOffsetY[kIndicatorStatusIndex],
  };

  // Error indicators matching historical panel
  inline static constexpr std::array<IndicatorLayout, 3> kErrorIndicators = {{
      {"INST",
       kIndicatorGroupBaseX[kIndicatorErrorIndex],
       kIndicatorGroupBaseY[kIndicatorErrorIndex] + 0 * kIndicatorStepY},
      {"ADD",
       kIndicatorGroupBaseX[kIndicatorErrorIndex],
       kIndicatorGroupBaseY[kIndicatorErrorIndex] + 1 * kIndicatorStepY},
      {"DIVIDE",
       kIndicatorGroupBaseX[kIndicatorErrorIndex],
       kIndicatorGroupBaseY[kIndicatorErrorIndex] + 2 * kIndicatorStepY},
  }};

  inline static constexpr TextLabel kErrorLabel = {
      "ERROR",
      kIndicatorGroupBaseX[kIndicatorErrorIndex] + kIndicatorGroupLabelOffsetX,
      kIndicatorGroupBaseY[kIndicatorErrorIndex] +
          kIndicatorGroupLabelOffsetY[kIndicatorErrorIndex],
  };
};

}  // namespace ct10::ui
