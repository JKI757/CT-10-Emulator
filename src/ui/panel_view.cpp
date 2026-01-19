#include "ui/panel_view.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "imgui.h"
#include "ui/panel_layout.h"

namespace ct10::ui {
namespace {

struct ButtonStyle {
  ImU32 base = 0;
  ImU32 hover = 0;
  ImU32 active = 0;
  ImU32 text = 0;
};

struct ToggleStyle {
  ImU32 base = 0;
  ImU32 on = 0;
  ImU32 off = 0;
  ImU32 border = 0;
  ImU32 text = 0;
};

ImU32 Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
  return IM_COL32(r, g, b, a);
}

ImU32 Color(const PanelLayout::ColorRGBA& color) {
  return Color(color.r, color.g, color.b, color.a);
}

bool DrawMomentaryButton(const char* id,
                         const char* label,
                         const Rect& rect,
                         float scale,
                         const ImVec2& origin,
                         const ButtonStyle& style,
                         ImDrawList* draw_list,
                         int label_offset_x = 0,
                         int label_offset_y = 0) {
  ImVec2 min(origin.x + rect.x * scale, origin.y + rect.y * scale);
  ImVec2 max(min.x + rect.w * scale, min.y + rect.h * scale);

  ImGui::SetCursorScreenPos(min);
  ImGui::InvisibleButton(id, ImVec2(rect.w * scale, rect.h * scale));

  bool pressed = ImGui::IsItemActivated();
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImU32 fill = style.base;
  if (active) {
    fill = style.active;
  } else if (hovered) {
    fill = style.hover;
  }

  draw_list->AddRectFilled(min, max, fill, PanelLayout::kMomentaryButtonCornerRadius * scale);
  draw_list->AddRect(min, max, Color(PanelLayout::kMomentaryButtonBorderColor),
                     PanelLayout::kMomentaryButtonCornerRadius * scale, 0,
                     PanelLayout::kMomentaryButtonBorderThickness);

  ImVec2 text_size = ImGui::CalcTextSize(label);
  ImVec2 text_pos(min.x + (rect.w * scale - text_size.x) * 0.5f +
                      label_offset_x * scale,
                  min.y + (rect.h * scale - text_size.y) * 0.5f +
                      label_offset_y * scale);
  draw_list->AddText(text_pos, style.text, label);

  return pressed;
}

bool DrawHoldButton(const char* id,
                    const char* label,
                    const Rect& rect,
                    float scale,
                    const ImVec2& origin,
                    const ButtonStyle& style,
                    ImDrawList* draw_list) {
  ImVec2 min(origin.x + rect.x * scale, origin.y + rect.y * scale);
  ImVec2 max(min.x + rect.w * scale, min.y + rect.h * scale);

  ImGui::SetCursorScreenPos(min);
  ImGui::InvisibleButton(id, ImVec2(rect.w * scale, rect.h * scale));

  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImU32 fill = style.base;
  if (active) {
    fill = style.active;
  } else if (hovered) {
    fill = style.hover;
  }

  draw_list->AddRectFilled(min, max, fill, PanelLayout::kMomentaryButtonCornerRadius * scale);
  draw_list->AddRect(min, max, Color(PanelLayout::kMomentaryButtonBorderColor),
                     PanelLayout::kMomentaryButtonCornerRadius * scale, 0,
                     PanelLayout::kMomentaryButtonBorderThickness);

  ImVec2 text_size = ImGui::CalcTextSize(label);
  ImVec2 text_pos(min.x + (rect.w * scale - text_size.x) * 0.5f,
                  min.y + (rect.h * scale - text_size.y) * 0.5f);
  draw_list->AddText(text_pos, style.text, label);

  return active;
}

bool DrawToggleSwitch(const char* id,
                      const Rect& rect,
                      float scale,
                      const ImVec2& origin,
                      bool on,
                      const ToggleStyle& style,
                      ImDrawList* draw_list) {
  ImVec2 min(origin.x + rect.x * scale, origin.y + rect.y * scale);
  ImVec2 max(min.x + rect.w * scale, min.y + rect.h * scale);

  ImGui::SetCursorScreenPos(min);
  ImGui::InvisibleButton(id, ImVec2(rect.w * scale, rect.h * scale));
  bool clicked = ImGui::IsItemClicked();
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImU32 fill = on ? style.on : style.off;
  if (hovered) {
    fill = style.base;
  }
  if (active) {
    fill = style.border;
  }

  draw_list->AddRectFilled(min, max, fill, PanelLayout::kToggleCornerRadius * scale);
  draw_list->AddRect(min, max, style.border, PanelLayout::kToggleCornerRadius * scale, 0,
                     PanelLayout::kToggleBorderThickness);

  return clicked;
}

bool DrawToggleWithLabel(const ToggleLayout& toggle,
                         float scale,
                         const ImVec2& origin,
                         bool on,
                         float label_offset,
                         const ToggleStyle& style,
                         ImDrawList* draw_list) {
  char id[32];
  std::snprintf(id, sizeof(id), "toggle_%s_%d_%d",
                toggle.label, toggle.rect.x, toggle.rect.y);
  bool clicked = DrawToggleSwitch(id, toggle.rect, scale, origin, on, style, draw_list);

  ImVec2 text_size = ImGui::CalcTextSize(toggle.label);
  float text_x = origin.x +
                 (toggle.rect.x + (toggle.rect.w - text_size.x) * 0.5f +
                  toggle.label_offset_x) *
                     scale;
  float text_y = origin.y +
                 (toggle.rect.y - label_offset + toggle.label_offset_y) * scale;
  draw_list->AddText(ImVec2(text_x, text_y), style.text, toggle.label);
  return clicked;
}

bool DrawInputLampButton(const ToggleLayout& toggle,
                         float scale,
                         const ImVec2& origin,
                         bool on,
                         float label_offset,
                         ImDrawList* draw_list) {
  ImVec2 min(origin.x + toggle.rect.x * scale, origin.y + toggle.rect.y * scale);
  ImVec2 max(min.x + toggle.rect.w * scale, min.y + toggle.rect.h * scale);

  ImGui::SetCursorScreenPos(min);
  ImGui::InvisibleButton(toggle.label, ImVec2(toggle.rect.w * scale,
                                              toggle.rect.h * scale));
  bool clicked = ImGui::IsItemClicked();

  ImU32 fill = on ? Color(PanelLayout::kInputLampOn) : Color(PanelLayout::kInputLampOff);
  draw_list->AddRectFilled(min, max, fill, PanelLayout::kLampCornerRadius * scale);
  draw_list->AddRect(min, max, Color(PanelLayout::kInputLampBorder),
                     PanelLayout::kLampCornerRadius * scale, 0,
                     PanelLayout::kInputLampBorderThickness);

  ImVec2 text_size = ImGui::CalcTextSize(toggle.label);
  float text_x = origin.x +
                 (toggle.rect.x + (toggle.rect.w - text_size.x) * 0.5f +
                  toggle.label_offset_x) *
                     scale;
  float text_y = origin.y +
                 (toggle.rect.y - label_offset + toggle.label_offset_y) * scale;
  // Dark text for cream background
  draw_list->AddText(ImVec2(text_x, text_y), Color(PanelLayout::kTextDark), toggle.label);
  return clicked;
}

void DrawGroupLabel(const TextLabel& label,
                    float scale,
                    const ImVec2& origin,
                    ImDrawList* draw_list,
                    ImU32 text_color = Color(PanelLayout::kTextLight)) {
  ImVec2 pos(origin.x + label.x * scale, origin.y + label.y * scale);
  draw_list->AddText(pos, text_color, label.label);
}

// Draw stacked text (multiple lines, centered at x position)
void DrawStackedText(float x,
                     float y,
                     const char* line1,
                     const char* line2,
                     float scale,
                     const ImVec2& origin,
                     ImU32 text_color,
                     ImDrawList* draw_list) {
  float line_height = ImGui::GetTextLineHeight();
  ImVec2 size1 = ImGui::CalcTextSize(line1);
  ImVec2 size2 = line2 ? ImGui::CalcTextSize(line2) : ImVec2(0, 0);
  float max_width = std::max(size1.x, size2.x);

  float x1 = origin.x + x * scale - size1.x * 0.5f;
  float y1 = origin.y + y * scale;
  draw_list->AddText(ImVec2(x1, y1), text_color, line1);

  if (line2) {
    float x2 = origin.x + x * scale - size2.x * 0.5f;
    float y2 = y1 + line_height;
    draw_list->AddText(ImVec2(x2, y2), text_color, line2);
  }
}

// Draw a horizontal line
void DrawHLine(float x1, float x2, float y,
               float scale,
               const ImVec2& origin,
               ImU32 color,
               float thickness,
               ImDrawList* draw_list) {
  ImVec2 p1(origin.x + x1 * scale, origin.y + y * scale);
  ImVec2 p2(origin.x + x2 * scale, origin.y + y * scale);
  draw_list->AddLine(p1, p2, color, thickness);
}

// Draw a vertical line
void DrawVLine(float x, float y1, float y2,
               float scale,
               const ImVec2& origin,
               ImU32 color,
               float thickness,
               ImDrawList* draw_list) {
  ImVec2 p1(origin.x + x * scale, origin.y + y1 * scale);
  ImVec2 p2(origin.x + x * scale, origin.y + y2 * scale);
  draw_list->AddLine(p1, p2, color, thickness);
}

// Draw a button without internal label (label drawn separately)
bool DrawUnlabeledButton(const char* id,
                         const Rect& rect,
                         float scale,
                         const ImVec2& origin,
                         const ButtonStyle& style,
                         ImDrawList* draw_list) {
  ImVec2 min(origin.x + rect.x * scale, origin.y + rect.y * scale);
  ImVec2 max(min.x + rect.w * scale, min.y + rect.h * scale);

  ImGui::SetCursorScreenPos(min);
  ImGui::InvisibleButton(id, ImVec2(rect.w * scale, rect.h * scale));

  bool pressed = ImGui::IsItemActivated();
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImU32 fill = style.base;
  if (active) {
    fill = style.active;
  } else if (hovered) {
    fill = style.hover;
  }

  draw_list->AddRectFilled(min, max, fill, PanelLayout::kButtonCornerRadius * scale);
  draw_list->AddRect(min, max, Color(PanelLayout::kButtonBorderColor),
                     PanelLayout::kButtonCornerRadius * scale, 0,
                     PanelLayout::kButtonBorderThickness);

  return pressed;
}

// Draw a hold button without internal label
bool DrawUnlabeledHoldButton(const char* id,
                             const Rect& rect,
                             float scale,
                             const ImVec2& origin,
                             const ButtonStyle& style,
                             ImDrawList* draw_list) {
  ImVec2 min(origin.x + rect.x * scale, origin.y + rect.y * scale);
  ImVec2 max(min.x + rect.w * scale, min.y + rect.h * scale);

  ImGui::SetCursorScreenPos(min);
  ImGui::InvisibleButton(id, ImVec2(rect.w * scale, rect.h * scale));

  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImU32 fill = style.base;
  if (active) {
    fill = style.active;
  } else if (hovered) {
    fill = style.hover;
  }

  draw_list->AddRectFilled(min, max, fill, PanelLayout::kButtonCornerRadius * scale);
  draw_list->AddRect(min, max, Color(PanelLayout::kButtonBorderColor),
                     PanelLayout::kButtonCornerRadius * scale, 0,
                     PanelLayout::kButtonBorderThickness);

  return active;
}

// Draw text centered above a rect
void DrawLabelAbove(const char* label,
                    const Rect& rect,
                    float y_offset,
                    float scale,
                    const ImVec2& origin,
                    ImU32 text_color,
                    ImDrawList* draw_list,
                    float x_offset = 0.0f) {
  ImVec2 text_size = ImGui::CalcTextSize(label);
  float text_x = origin.x + (rect.x + rect.w * 0.5f + x_offset) * scale -
                 text_size.x * 0.5f;
  float text_y = origin.y + (rect.y - y_offset) * scale;
  draw_list->AddText(ImVec2(text_x, text_y), text_color, label);
}

void DrawLampBox(const ImVec2& origin,
                 float scale,
                 float x,
                 float y,
                 float size,
                 bool on,
                 ImDrawList* draw_list) {
  ImVec2 min(origin.x + x * scale, origin.y + y * scale);
  ImVec2 max(min.x + size * scale, min.y + size * scale);
  ImU32 fill = on ? Color(PanelLayout::kLampOn) : Color(PanelLayout::kLampOff);
  draw_list->AddRectFilled(min, max, fill, PanelLayout::kLampCornerRadius * scale);
  draw_list->AddRect(min, max, Color(PanelLayout::kLampBorder),
                     PanelLayout::kLampCornerRadius * scale, 0,
                     PanelLayout::kLampBorderThickness);
}

void DrawLampStrip(const ImVec2& origin,
                   float scale,
                   const LampStripLayout& layout,
                   uint16_t value,
                   bool lamp_test,
                   ImDrawList* draw_list) {
  float lamp = PanelLayout::kLampSize;
  float gap = PanelLayout::kLampGap;
  float group_gap = PanelLayout::kLampGroupGap;

  ImVec2 label_pos(origin.x + layout.x * scale,
                   origin.y + (layout.y - PanelLayout::kLampStripLabelOffset) * scale);
  draw_list->AddText(label_pos, Color(PanelLayout::kTextLight), layout.label);

  int bits = layout.bits;
  int gap_after = layout.gap_after;
  for (int i = 0; i < bits; ++i) {
    int bit = (bits - 1) - i;
    float offset = i * (lamp + gap);
    if (gap_after > 0 && i >= gap_after) {
      offset += group_gap;
    }
    float x = layout.x + offset;
    float y = layout.y;
    bool on = lamp_test ? true : ((value >> bit) & 0x1u);
    DrawLampBox(origin, scale, x, y, lamp, on, draw_list);

    char bit_label[4];
    std::snprintf(bit_label, sizeof(bit_label), "%d", bit);
    ImVec2 text_size = ImGui::CalcTextSize(bit_label);
    float text_x = origin.x + (x + (lamp - text_size.x) * 0.5f) * scale;
    float text_y = origin.y + (y - PanelLayout::kLampBitLabelOffset) * scale;
    draw_list->AddText(ImVec2(text_x, text_y), Color(PanelLayout::kTextMuted), bit_label);
  }
}

void DrawIndicators(const ImVec2& origin,
                    float scale,
                    const IndicatorLayout* indicators,
                    size_t count,
                    const bool* values,
                    bool lamp_test,
                    ImDrawList* draw_list) {
  float size = PanelLayout::kLampSize;
  for (size_t i = 0; i < count; ++i) {
    bool on = lamp_test ? true : values[i];
    DrawLampBox(origin, scale,
                static_cast<float>(indicators[i].x),
                static_cast<float>(indicators[i].y),
                size,
                on,
                draw_list);
    ImVec2 text_pos(origin.x + (indicators[i].x + size + PanelLayout::kIndicatorLabelOffsetX) *
                                      scale,
                    origin.y +
                        (indicators[i].y - PanelLayout::kIndicatorLabelOffsetY) * scale);
    draw_list->AddText(text_pos, Color(PanelLayout::kTextIndicator), indicators[i].label);
  }
}

void DrawPanelBackground(const ImVec2& origin,
                         const ImVec2& panel_size,
                         ImU32 top_left,
                         ImU32 top_right,
                         ImU32 bottom_right,
                         ImU32 bottom_left,
                         ImDrawList* draw_list) {
  ImVec2 panel_max(origin.x + panel_size.x, origin.y + panel_size.y);
  draw_list->AddRectFilledMultiColor(origin, panel_max, top_left, top_right,
                                     bottom_right, bottom_left);
  draw_list->AddRect(origin, panel_max, Color(PanelLayout::kPanelBorderColor), 0.0f, 0,
                     PanelLayout::kPanelBorderThickness);
}

void DrawDisplayPanel(core::MachineState& state,
                      const ImVec2& layout_origin,
                      const ImVec2& panel_origin,
                      float scale,
                      float panel_width_units,
                      float panel_height_units,
                      ImDrawList* draw_list) {
  auto strip_width = [](const LampStripLayout& layout) -> float {
    float lamp = PanelLayout::kLampSize;
    float gap = PanelLayout::kLampGap;
    float group_gap = PanelLayout::kLampGroupGap;
    if (layout.bits <= 0) {
      return 0.0f;
    }
    float width = layout.bits * (lamp + gap) - gap;
    if (layout.gap_after > 0) {
      width += group_gap;
    }
    return width;
  };

  DrawGroupLabel(PanelLayout::kCondLabel, scale, layout_origin, draw_list);
  DrawGroupLabel(PanelLayout::kStatusLabel, scale, layout_origin, draw_list);
  DrawGroupLabel(PanelLayout::kErrorLabel, scale, layout_origin, draw_list);

  bool power_on = state.panel_input.power_on;
  bool lamp_test = power_on && state.panel_input.lamp_test;

  const bool cond_values[] = {
      power_on ? state.flags.greater : false,
      power_on ? state.flags.zero : false,
      power_on ? state.flags.less : false,
      power_on ? state.flags.carry : false,
  };
  DrawIndicators(layout_origin, scale,
                 PanelLayout::kCondIndicators.data(),
                 PanelLayout::kCondIndicators.size(),
                 cond_values,
                 lamp_test,
                 draw_list);

  const bool status_values[] = {
      power_on ? state.status.interrupt : false,
      power_on ? state.status.sense : false,
      power_on ? state.status.flag : false,
      power_on ? state.status.wait : false,
  };
  DrawIndicators(layout_origin, scale,
                 PanelLayout::kStatusIndicators.data(),
                 PanelLayout::kStatusIndicators.size(),
                 status_values,
                 lamp_test,
                 draw_list);

  const bool error_values[] = {
      power_on ? state.flags.inst_error : false,
      power_on ? state.flags.add_overflow : false,
      power_on ? state.flags.divide_overflow : false,
  };
  DrawIndicators(layout_origin, scale,
                 PanelLayout::kErrorIndicators.data(),
                 PanelLayout::kErrorIndicators.size(),
                 error_values,
                 lamp_test,
                 draw_list);

  const uint16_t lamp_values[] = {
      static_cast<uint16_t>(power_on ? state.accumulator.value() : 0),
      static_cast<uint16_t>(power_on ? state.quotient.value() : 0),
      static_cast<uint16_t>(power_on ? state.buffer.value() : 0),
      static_cast<uint16_t>(power_on ? state.index.value() : 0),
      static_cast<uint16_t>(power_on ? state.opcode.value() : 0),
      static_cast<uint16_t>(power_on ? state.mar.value() : 0),
      static_cast<uint16_t>(power_on ? state.countdown.value() : 0),
      static_cast<uint16_t>(power_on ? state.distributor.value() : 0),
      static_cast<uint16_t>(power_on ? state.par.value() : 0),
  };

  // Decorative lines for display panel (matching historical panel)
  ImU32 display_line_color = Color(PanelLayout::kDisplayLineColor);
  for (const auto& line : PanelLayout::kDisplayPanelLines) {
    DrawHLine(line.x1, line.x2, line.y, scale, layout_origin,
              display_line_color, line.thickness, draw_list);
  }

  for (size_t i = 0; i < PanelLayout::kLampStrips.size(); ++i) {
    if (i >= static_cast<size_t>(PanelLayout::kTopLampStripCount)) {
      break;
    }
    DrawLampStrip(layout_origin, scale, PanelLayout::kLampStrips[i], lamp_values[i], lamp_test,
                  draw_list);
  }

  LampStripLayout countdown = PanelLayout::kLampStrips[PanelLayout::kCountdownStripIndex];
  LampStripLayout distributor = PanelLayout::kLampStrips[PanelLayout::kDistributorStripIndex];
  LampStripLayout prog_addr = PanelLayout::kLampStrips[PanelLayout::kProgramAddressStripIndex];
  float bottom_y = panel_height_units - PanelLayout::kDisplayBottomMargin;
  countdown.y = static_cast<int>(bottom_y);
  distributor.y = static_cast<int>(bottom_y);
  prog_addr.y = static_cast<int>(bottom_y);

  float w1 = strip_width(countdown);
  float w2 = strip_width(distributor);
  float w3 = strip_width(prog_addr);
  float total = w1 + w2 + w3;
  float spacing = (panel_width_units - total) / PanelLayout::kDisplayBottomSpacingSlots;
  if (spacing < PanelLayout::kDisplayBottomMinSpacing) {
    spacing = PanelLayout::kDisplayBottomMinSpacing;
  }
  float x1 = spacing;
  float x2 = x1 + w1 + spacing;
  float x3 = x2 + w2 + spacing;
  countdown.x = static_cast<int>(x1);
  distributor.x = static_cast<int>(x2);
  prog_addr.x = static_cast<int>(x3);

  DrawLampStrip(panel_origin, scale, countdown, lamp_values[6], lamp_test, draw_list);
  DrawLampStrip(panel_origin, scale, distributor, lamp_values[7], lamp_test, draw_list);
  DrawLampStrip(panel_origin, scale, prog_addr, lamp_values[8], lamp_test, draw_list);
}

void DrawInputPanel(core::MachineState& state,
                    const ImVec2& origin,
                    float scale,
                    ImDrawList* draw_list) {
  // Light cream/white keypad buttons with dark text (matching historical panel)
  ButtonStyle keypad_style{Color(PanelLayout::kKeypadButtonBase),
                           Color(PanelLayout::kKeypadButtonHover),
                           Color(PanelLayout::kKeypadButtonActive),
                           Color(PanelLayout::kKeypadButtonText)};
  // Default control button style
  ButtonStyle control_style{Color(PanelLayout::kControlButtonBase),
                            Color(PanelLayout::kControlButtonHover),
                            Color(PanelLayout::kControlButtonActive),
                            Color(PanelLayout::kControlButtonText)};
  // Colored control buttons matching historical panel
  ButtonStyle clear_style{Color(PanelLayout::kClearButtonBase),
                          Color(PanelLayout::kClearButtonHover),
                          Color(PanelLayout::kClearButtonActive),
                          Color(PanelLayout::kClearButtonText)};
  ButtonStyle stop_style{Color(PanelLayout::kStopButtonBase),
                         Color(PanelLayout::kStopButtonHover),
                         Color(PanelLayout::kStopButtonActive),
                         Color(PanelLayout::kStopButtonText)};
  ButtonStyle start_style{Color(PanelLayout::kStartButtonBase),
                          Color(PanelLayout::kStartButtonHover),
                          Color(PanelLayout::kStartButtonActive),
                          Color(PanelLayout::kStartButtonText)};
  // Toggle switches with darker appearance
  ToggleStyle toggle_style{Color(PanelLayout::kToggleBase),
                           Color(PanelLayout::kToggleOn),
                           Color(PanelLayout::kToggleOff),
                           Color(PanelLayout::kToggleBorder),
                           Color(PanelLayout::kToggleText)};
  // Dark text color for cream background
  ImU32 dark_text = Color(PanelLayout::kTextDark);
  // Line color for decorations
  ImU32 line_color = Color(PanelLayout::kInputLineColor);

  // === LEFT SIDE: POWER, COM-TRAN TEN, LAMP TEST ===

  // POWER - label above switch
  DrawStackedText(PanelLayout::kPowerLabelX, PanelLayout::kPowerLabelY,
                  PanelLayout::kPowerLabelText, nullptr, scale, origin, dark_text, draw_list);
  if (DrawToggleSwitch("power_switch", PanelLayout::kPower.rect, scale, origin,
                       state.panel_input.power_on, toggle_style, draw_list)) {
    state.panel_input.power_on = !state.panel_input.power_on;
  }

  // LAMP TEST - stacked text above button
  DrawStackedText(PanelLayout::kLampTestLabelX, PanelLayout::kLampTestLabelY,
                  PanelLayout::kLampTestLabelLine1, PanelLayout::kLampTestLabelLine2, scale,
                  origin, dark_text, draw_list);
  state.panel_input.lamp_test = DrawUnlabeledHoldButton("lamp_test",
                                                        PanelLayout::kLampTest.rect, scale, origin,
                                                        control_style, draw_list);
  bool lamp_test = state.panel_input.power_on && state.panel_input.lamp_test;

  // === HEX KEYPAD ===
  for (const auto& key : PanelLayout::kHexKeypad) {
    char label[2] = {key.label, 0};
    char id[16];
    std::snprintf(id, sizeof(id), "key_%c", key.label);
    if (DrawMomentaryButton(id, label, key.rect, scale, origin, keypad_style, draw_list,
                            key.label_offset_x, key.label_offset_y)) {
      uint8_t value = 0;
      if (key.label >= '0' && key.label <= '9') {
        value = static_cast<uint8_t>(key.label - '0');
      } else {
        value = static_cast<uint8_t>(10 + (key.label - 'A'));
      }
      state.panel_input.key_pressed = true;
      state.panel_input.key_value = value;
      state.panel_input.last_key = value;
      state.panel_input.has_last_key = true;
      uint16_t high = static_cast<uint16_t>(state.panel_input.input_switches & 0x300);
      uint16_t low = static_cast<uint16_t>(state.panel_input.input_switches & 0xFF);
      low = static_cast<uint16_t>(((low << 4) & 0xF0) | value);
      state.panel_input.input_switches = static_cast<uint16_t>(high | low);
    }
  }

  // === INPUT SWITCHES ROW with decorative lines ===
  // Draw INPUT label and lines
  DrawHLine(PanelLayout::kInputHeaderLines[0].x1,
            PanelLayout::kInputHeaderLines[0].x2,
            PanelLayout::kInputHeaderLines[0].y, scale, origin, line_color,
            PanelLayout::kInputHeaderLines[0].thickness, draw_list);
  DrawGroupLabel(PanelLayout::kInputGroup.label, scale, origin, draw_list, dark_text);
  DrawHLine(PanelLayout::kInputHeaderLines[1].x1,
            PanelLayout::kInputHeaderLines[1].x2,
            PanelLayout::kInputHeaderLines[1].y, scale, origin, line_color,
            PanelLayout::kInputHeaderLines[1].thickness, draw_list);

  for (size_t i = 0; i < PanelLayout::kInputGroup.toggles.size(); ++i) {
    const auto& toggle = PanelLayout::kInputGroup.toggles[i];
    bool on = (state.panel_input.input_switches >> (9 - i)) & 0x1u;
    bool lit = lamp_test || on;
    if (DrawInputLampButton(toggle, scale, origin, lit,
                            PanelLayout::kInputGroup.label_offset, draw_list)) {
      uint16_t mask = static_cast<uint16_t>(1u << (9 - i));
      state.panel_input.input_switches ^= mask;
    }
  }

  // === RESET button ===
  DrawLabelAbove(PanelLayout::kResetLabelText, PanelLayout::kReset.rect,
                 PanelLayout::kResetLabelOffsetY, scale, origin, dark_text, draw_list,
                 PanelLayout::kResetLabelOffsetX);
  if (DrawUnlabeledButton("reset", PanelLayout::kReset.rect, scale, origin, control_style,
                          draw_list)) {
    state.panel_input.reset = true;
  }

  // === I/O MODE section (Row 1, left) ===
  DrawHLine(PanelLayout::kIoModeLines[0].x1,
            PanelLayout::kIoModeLines[0].x2,
            PanelLayout::kIoModeLines[0].y, scale, origin, line_color,
            PanelLayout::kIoModeLines[0].thickness, draw_list);
  DrawGroupLabel(PanelLayout::kIoModeGroup.label, scale, origin, draw_list, dark_text);
  DrawHLine(PanelLayout::kIoModeLines[1].x1,
            PanelLayout::kIoModeLines[1].x2,
            PanelLayout::kIoModeLines[1].y, scale, origin, line_color,
            PanelLayout::kIoModeLines[1].thickness, draw_list);
  for (size_t i = 0; i < PanelLayout::kIoModeGroup.toggles.size(); ++i) {
    bool on = state.panel_input.io_mode == i;
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kIoModeGroup.toggles[i], scale, origin, lit,
                            PanelLayout::kIoModeGroup.label_offset, toggle_style, draw_list)) {
      state.panel_input.io_mode = static_cast<uint8_t>(i);
    }
  }

  // === MODE section (Row 2, includes RPT/SINGLE) ===
  DrawHLine(PanelLayout::kModeLines[0].x1,
            PanelLayout::kModeLines[0].x2,
            PanelLayout::kModeLines[0].y, scale, origin, line_color,
            PanelLayout::kModeLines[0].thickness, draw_list);
  DrawGroupLabel(PanelLayout::kModeGroup.label, scale, origin, draw_list, dark_text);
  DrawHLine(PanelLayout::kModeLines[1].x1,
            PanelLayout::kModeLines[1].x2,
            PanelLayout::kModeLines[1].y, scale, origin, line_color,
            PanelLayout::kModeLines[1].thickness, draw_list);
  for (size_t i = 0; i < PanelLayout::kModeGroup.toggles.size(); ++i) {
    bool on = false;
    if (i < 4) {
      on = state.panel_input.mode == i;
    } else if (i == 4) {
      on = state.panel_input.rpt;
    } else {
      on = state.panel_input.sense;
    }
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kModeGroup.toggles[i], scale, origin, lit,
                            PanelLayout::kModeGroup.label_offset, toggle_style, draw_list)) {
      if (i < 4) {
        state.panel_input.mode = static_cast<uint8_t>(i);
      } else if (i == 4) {
        state.panel_input.rpt = !state.panel_input.rpt;
      } else {
        state.panel_input.sense = !state.panel_input.sense;
      }
    }
  }

  // === ERROR BYPASS section (Row 3, left) ===
  DrawHLine(PanelLayout::kErrorBypassLines[0].x1,
            PanelLayout::kErrorBypassLines[0].x2,
            PanelLayout::kErrorBypassLines[0].y, scale, origin, line_color,
            PanelLayout::kErrorBypassLines[0].thickness, draw_list);
  DrawGroupLabel(PanelLayout::kErrorBypassGroup.label, scale, origin, draw_list, dark_text);
  DrawHLine(PanelLayout::kErrorBypassLines[1].x1,
            PanelLayout::kErrorBypassLines[1].x2,
            PanelLayout::kErrorBypassLines[1].y, scale, origin, line_color,
            PanelLayout::kErrorBypassLines[1].thickness, draw_list);
  for (size_t i = 0; i < PanelLayout::kErrorBypassGroup.toggles.size(); ++i) {
    bool on = false;
    if (i == 0) {
      on = state.panel_input.error_inst;
    } else if (i == 1) {
      on = state.panel_input.error_add;
    } else {
      on = state.panel_input.error_div;
    }
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kErrorBypassGroup.toggles[i], scale, origin, lit,
                            PanelLayout::kErrorBypassGroup.label_offset, toggle_style,
                            draw_list)) {
      if (i == 0) {
        state.panel_input.error_inst = !state.panel_input.error_inst;
      } else if (i == 1) {
        state.panel_input.error_add = !state.panel_input.error_add;
      } else {
        state.panel_input.error_div = !state.panel_input.error_div;
      }
    }
  }

  // === I/O section (READ/INTRPT, WRITE/BLOCK) ===
  DrawGroupLabel(PanelLayout::kIoGroup.label, scale, origin, draw_list, dark_text);
  bool read_intrp = state.panel_input.io_read && state.panel_input.io_intrp;
  bool write_block = state.panel_input.io_write && state.panel_input.io_block;

  auto draw_io_label = [&](const ToggleLayout& toggle,
                           const char* line1,
                           const char* line2,
                           int offset_y) {
    float line_height = ImGui::GetTextLineHeight();
    float label_y =
        toggle.rect.y - PanelLayout::kButtonLabelOffsetY - line_height + offset_y;
    float label_x = toggle.rect.x + (toggle.rect.w * 0.5f);
    DrawStackedText(label_x, label_y, line1, line2, scale, origin, dark_text, draw_list);
  };

  const ToggleLayout& io_read = PanelLayout::kIoGroup.toggles[0];
  const ToggleLayout& io_write = PanelLayout::kIoGroup.toggles[1];

  draw_io_label(io_read, PanelLayout::kIoReadLabelLine1, PanelLayout::kIoReadLabelLine2,
                PanelLayout::kIoReadLabelOffsetY);
  draw_io_label(io_write, PanelLayout::kIoWriteLabelLine1, PanelLayout::kIoWriteLabelLine2,
                PanelLayout::kIoWriteLabelOffsetY);

  bool read_lit = lamp_test || read_intrp;
  if (DrawToggleSwitch("io_read_intrp", io_read.rect, scale, origin, read_lit,
                       toggle_style, draw_list)) {
    read_intrp = !read_intrp;
    state.panel_input.io_read = read_intrp;
    state.panel_input.io_intrp = read_intrp;
    if (read_intrp) {
      state.panel_input.io_write = false;
      state.panel_input.io_block = false;
      write_block = false;
    }
  }

  bool write_lit = lamp_test || write_block;
  if (DrawToggleSwitch("io_write_block", io_write.rect, scale, origin, write_lit,
                       toggle_style, draw_list)) {
    write_block = !write_block;
    state.panel_input.io_write = write_block;
    state.panel_input.io_block = write_block;
    if (write_block) {
      state.panel_input.io_read = false;
      state.panel_input.io_intrp = false;
    }
  }

  // === CONTROL section (Row 1, right) ===
  DrawHLine(PanelLayout::kControlLines[0].x1,
            PanelLayout::kControlLines[0].x2,
            PanelLayout::kControlLines[0].y, scale, origin, line_color,
            PanelLayout::kControlLines[0].thickness, draw_list);
  DrawGroupLabel(PanelLayout::kControlGroup.label, scale, origin, draw_list, dark_text);
  DrawHLine(PanelLayout::kControlLines[1].x1,
            PanelLayout::kControlLines[1].x2,
            PanelLayout::kControlLines[1].y, scale, origin, line_color,
            PanelLayout::kControlLines[1].thickness, draw_list);

  // Draw control buttons with labels above and individual colors
  for (const auto& control : PanelLayout::kControlGroup.controls) {
    char id[32];
    std::snprintf(id, sizeof(id), "control_%s", control.name);
    const ButtonStyle* btn_style = &control_style;
    if (std::strcmp(control.name, "CLEAR") == 0) {
      btn_style = &clear_style;
    } else if (std::strcmp(control.name, "STOP") == 0) {
      btn_style = &stop_style;
    } else if (std::strcmp(control.name, "START") == 0) {
      btn_style = &start_style;
    }
    // Draw label above button
    DrawLabelAbove(control.name, control.rect,
                   PanelLayout::kControlGroup.label_offset + control.label_offset_y,
                   scale, origin, dark_text, draw_list, control.label_offset_x);
    if (DrawUnlabeledButton(id, control.rect, scale, origin, *btn_style, draw_list)) {
      if (std::strcmp(control.name, "START") == 0) {
        state.panel_input.start = true;
      } else if (std::strcmp(control.name, "STOP") == 0) {
        state.panel_input.stop = true;
      } else if (std::strcmp(control.name, "CLEAR") == 0) {
        state.panel_input.clear = true;
      }
    }
  }

  // === LOAD section - register load buttons ===
  DrawGroupLabel(PanelLayout::kRegisterLoadGroup.label, scale, origin, draw_list, dark_text);
  for (const auto& load : PanelLayout::kRegisterLoadGroup.buttons) {
    char id[32];
    std::snprintf(id, sizeof(id), "load_%s", load.name);
    // Use buttons with internal labels (matching historical panel style)
    if (DrawMomentaryButton(id, load.name, load.rect, scale, origin, control_style, draw_list,
                            load.label_offset_x, load.label_offset_y)) {
      state.panel_input.load_pressed = true;
      if (std::strcmp(load.name, "A") == 0) {
        state.panel_input.load_target = core::LoadTarget::Accumulator;
      } else if (std::strcmp(load.name, "B") == 0) {
        state.panel_input.load_target = core::LoadTarget::Buffer;
      } else if (std::strcmp(load.name, "C") == 0) {
        state.panel_input.load_target = core::LoadTarget::Countdown;
      } else if (std::strcmp(load.name, "D") == 0) {
        state.panel_input.load_target = core::LoadTarget::Distributor;
      } else if (std::strcmp(load.name, "OP") == 0) {
        state.panel_input.load_target = core::LoadTarget::Opcode;
      } else if (std::strcmp(load.name, "MAR") == 0) {
        state.panel_input.load_target = core::LoadTarget::Mar;
      } else if (std::strcmp(load.name, "PAR") == 0) {
        state.panel_input.load_target = core::LoadTarget::Par;
      } else if (std::strcmp(load.name, "Q") == 0) {
        state.panel_input.load_target = core::LoadTarget::Quotient;
      } else if (std::strcmp(load.name, "X") == 0) {
        state.panel_input.load_target = core::LoadTarget::Index;
      }
    }
  }

  // === MEMORY section (RD WT) ===
  DrawGroupLabel(PanelLayout::kMemoryGroup.label, scale, origin, draw_list, dark_text);
  for (size_t i = 0; i < PanelLayout::kMemoryGroup.toggles.size(); ++i) {
    bool on = (i == 0) ? state.panel_input.mem_read : state.panel_input.mem_write;
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kMemoryGroup.toggles[i], scale, origin, lit,
                            PanelLayout::kMemoryGroup.label_offset, toggle_style,
                            draw_list)) {
      if (i == 0) {
        state.panel_input.mem_read = !state.panel_input.mem_read;
        if (state.panel_input.mem_read) {
          state.panel_input.mem_write = false;
        }
      } else {
        state.panel_input.mem_write = !state.panel_input.mem_write;
        if (state.panel_input.mem_write) {
          state.panel_input.mem_read = false;
        }
      }
    }
  }

  // === BRANDING ===
  // COM-TRAN TEN - prominent branding on left side below POWER
  DrawStackedText(PanelLayout::kBrandLabelX, PanelLayout::kBrandLabelY,
                  PanelLayout::kBrandLabelText, nullptr, scale, origin, dark_text, draw_list);
  // DIGIAC corporation - top right corner (above RESET)
  draw_list->AddText(ImVec2(origin.x + PanelLayout::kDigiaLabelX * scale,
                            origin.y + PanelLayout::kDigiaLabelY * scale),
                     dark_text, PanelLayout::kDigiaLabelText);
}

}  // namespace

PanelView::PanelView(ImFont* display_font, ImFont* input_font)
    : display_font_(display_font), input_font_(input_font) {}

void PanelView::Draw(core::MachineState& state) const {
  ImVec2 display = ImGui::GetIO().DisplaySize;
  float right_column = PanelLayout::kRightColumnWidth;
  float panel_width = std::max(PanelLayout::kPanelWidthMin,
                               display.x - right_column - PanelLayout::kPanelWidthPadding);
  float available_height = std::max(PanelLayout::kPanelHeightMin,
                                    display.y - PanelLayout::kPanelHeightPadding);
  float gap = PanelLayout::kPanelGap;
  float top_height = std::max(PanelLayout::kTopHeightMin,
                              available_height * PanelLayout::kTopHeightRatio);
  float bottom_height = std::max(PanelLayout::kBottomHeightMin,
                                 available_height - top_height - gap);
  if (top_height + bottom_height + gap > available_height) {
    bottom_height = std::max(PanelLayout::kBottomHeightClampMin,
                             available_height - top_height - gap);
  }

  float display_x = PanelLayout::kPanelEdgeMargin;
  float input_x = PanelLayout::kPanelEdgeMargin;
  float display_y = PanelLayout::kPanelEdgeMargin;
  float input_y = display_y + top_height + gap;

  ImGui::SetNextWindowPos(ImVec2(input_x, input_y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(panel_width, bottom_height), ImGuiCond_Always);
  ImGui::Begin("Input Panel", nullptr,
               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

  if (input_font_) {
    ImGui::PushFont(input_font_);
  }

  ImVec2 input_origin = ImGui::GetCursorScreenPos();
  ImVec2 input_avail = ImGui::GetContentRegionAvail();
  float input_margin = PanelLayout::kPanelInnerMargin;
  float input_scale = std::min(
      (input_avail.x - PanelLayout::kPanelInnerMarginScale * input_margin) /
          PanelLayout::kInputWidth,
      (input_avail.y - PanelLayout::kPanelInnerMarginScale * input_margin) /
          PanelLayout::kInputHeight);
  input_scale = std::max(input_scale, PanelLayout::kPanelScaleMin);

  ImVec2 input_layout_origin(input_origin.x + input_margin,
                             input_origin.y + input_margin);
  ImDrawList* input_draw_list = ImGui::GetWindowDrawList();
  // Cream/beige metal panel background matching historical panel
  DrawPanelBackground(input_origin, input_avail,
                      Color(PanelLayout::kInputPanelTopLeft),
                      Color(PanelLayout::kInputPanelTopRight),
                      Color(PanelLayout::kInputPanelBottomRight),
                      Color(PanelLayout::kInputPanelBottomLeft),
                      input_draw_list);

  DrawInputPanel(state, input_layout_origin, input_scale, input_draw_list);

  ImGui::Dummy(input_avail);
  if (input_font_) {
    ImGui::PopFont();
  }
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(display_x, display_y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(panel_width, top_height), ImGuiCond_Always);
  ImGui::Begin("Display Panel", nullptr,
               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

  if (display_font_) {
    ImGui::PushFont(display_font_);
  }

  ImVec2 display_origin = ImGui::GetCursorScreenPos();
  ImVec2 display_avail = ImGui::GetContentRegionAvail();
  float margin = PanelLayout::kPanelInnerMargin;
  float display_scale = std::min(
      (display_avail.x - PanelLayout::kPanelInnerMarginScale * margin) /
          PanelLayout::kDisplayWidth,
      (display_avail.y - PanelLayout::kPanelInnerMarginScale * margin) /
          PanelLayout::kDisplayHeight);
  display_scale = std::max(display_scale, PanelLayout::kPanelScaleMin);

  float display_units_w = display_avail.x / display_scale;
  float display_units_h = display_avail.y / display_scale;
  ImVec2 display_layout_origin(display_origin.x + margin,
                               display_origin.y + margin);
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  // Dark charcoal/slate background matching historical panel
  DrawPanelBackground(display_origin, display_avail,
                      Color(PanelLayout::kDisplayPanelTopLeft),
                      Color(PanelLayout::kDisplayPanelTopRight),
                      Color(PanelLayout::kDisplayPanelBottomRight),
                      Color(PanelLayout::kDisplayPanelBottomLeft),
                      draw_list);

  DrawDisplayPanel(state, display_layout_origin, display_origin, display_scale,
                   display_units_w, display_units_h, draw_list);

  ImGui::Dummy(display_avail);
  if (display_font_) {
    ImGui::PopFont();
  }
  ImGui::End();
}

}  // namespace ct10::ui
