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

bool DrawMomentaryButton(const char* id,
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

  bool pressed = ImGui::IsItemActivated();
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImU32 fill = style.base;
  if (active) {
    fill = style.active;
  } else if (hovered) {
    fill = style.hover;
  }

  draw_list->AddRectFilled(min, max, fill, 6.0f * scale);
  draw_list->AddRect(min, max, Color(18, 18, 18), 6.0f * scale, 0, 1.5f);

  ImVec2 text_size = ImGui::CalcTextSize(label);
  ImVec2 text_pos(min.x + (rect.w * scale - text_size.x) * 0.5f,
                  min.y + (rect.h * scale - text_size.y) * 0.5f);
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

  draw_list->AddRectFilled(min, max, fill, 6.0f * scale);
  draw_list->AddRect(min, max, Color(18, 18, 18), 6.0f * scale, 0, 1.5f);

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

  draw_list->AddRectFilled(min, max, fill, 3.0f * scale);
  draw_list->AddRect(min, max, style.border, 3.0f * scale, 0, 1.2f);

  return clicked;
}

bool DrawToggleWithLabel(const ToggleLayout& toggle,
                         float scale,
                         const ImVec2& origin,
                         bool on,
                         const ToggleStyle& style,
                         ImDrawList* draw_list) {
  char id[32];
  std::snprintf(id, sizeof(id), "toggle_%s_%d_%d",
                toggle.label, toggle.rect.x, toggle.rect.y);
  bool clicked = DrawToggleSwitch(id, toggle.rect, scale, origin, on, style, draw_list);

  ImVec2 text_size = ImGui::CalcTextSize(toggle.label);
  float text_x = origin.x + (toggle.rect.x + (toggle.rect.w - text_size.x) * 0.5f) * scale;
  float text_y = origin.y + (toggle.rect.y - 18.0f) * scale;
  draw_list->AddText(ImVec2(text_x, text_y), style.text, toggle.label);
  return clicked;
}

bool DrawInputLampButton(const ToggleLayout& toggle,
                         float scale,
                         const ImVec2& origin,
                         bool on,
                         ImDrawList* draw_list) {
  ImVec2 min(origin.x + toggle.rect.x * scale, origin.y + toggle.rect.y * scale);
  ImVec2 max(min.x + toggle.rect.w * scale, min.y + toggle.rect.h * scale);

  ImGui::SetCursorScreenPos(min);
  ImGui::InvisibleButton(toggle.label, ImVec2(toggle.rect.w * scale,
                                              toggle.rect.h * scale));
  bool clicked = ImGui::IsItemClicked();

  ImU32 fill = on ? Color(255, 196, 64) : Color(50, 36, 22);
  draw_list->AddRectFilled(min, max, fill, 2.0f * scale);
  draw_list->AddRect(min, max, Color(26, 18, 12), 2.0f * scale, 0, 1.2f);

  ImVec2 text_size = ImGui::CalcTextSize(toggle.label);
  float text_x = origin.x + (toggle.rect.x + (toggle.rect.w - text_size.x) * 0.5f) * scale;
  float text_y = origin.y + (toggle.rect.y - 18.0f) * scale;
  // Dark text for cream background
  draw_list->AddText(ImVec2(text_x, text_y), Color(40, 35, 30), toggle.label);
  return clicked;
}

void DrawGroupLabel(const TextLabel& label,
                    float scale,
                    const ImVec2& origin,
                    ImDrawList* draw_list,
                    ImU32 text_color = Color(230, 230, 230)) {
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

  draw_list->AddRectFilled(min, max, fill, 4.0f * scale);
  draw_list->AddRect(min, max, Color(30, 30, 30), 4.0f * scale, 0, 1.5f);

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

  draw_list->AddRectFilled(min, max, fill, 4.0f * scale);
  draw_list->AddRect(min, max, Color(30, 30, 30), 4.0f * scale, 0, 1.5f);

  return active;
}

// Draw text centered above a rect
void DrawLabelAbove(const char* label,
                    const Rect& rect,
                    float y_offset,
                    float scale,
                    const ImVec2& origin,
                    ImU32 text_color,
                    ImDrawList* draw_list) {
  ImVec2 text_size = ImGui::CalcTextSize(label);
  float text_x = origin.x + (rect.x + rect.w * 0.5f) * scale - text_size.x * 0.5f;
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
  ImU32 fill = on ? Color(255, 196, 64) : Color(54, 38, 22);
  draw_list->AddRectFilled(min, max, fill, 2.0f * scale);
  draw_list->AddRect(min, max, Color(30, 20, 10), 2.0f * scale, 0, 1.5f);
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

  ImVec2 label_pos(origin.x + layout.x * scale, origin.y + (layout.y - 28) * scale);
  draw_list->AddText(label_pos, Color(230, 230, 230), layout.label);

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
    float text_y = origin.y + (y - 18.0f) * scale;
    draw_list->AddText(ImVec2(text_x, text_y), Color(200, 200, 200), bit_label);
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
    ImVec2 text_pos(origin.x + (indicators[i].x + size + 8.0f) * scale,
                    origin.y + (indicators[i].y - 2.0f) * scale);
    draw_list->AddText(text_pos, Color(220, 220, 220), indicators[i].label);
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
  draw_list->AddRect(origin, panel_max, Color(18, 18, 18), 0.0f, 0, 2.0f);
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
  ImU32 display_line_color = Color(180, 180, 180);

  // Lines for top row (ACCUMULATOR and QUOTIENT)
  DrawHLine(130.0f, 195.0f, 100.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);
  DrawHLine(370.0f, 420.0f, 100.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);
  DrawHLine(465.0f, 530.0f, 100.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);

  // Lines for second row (BUFFER and INDEX)
  DrawHLine(130.0f, 215.0f, 180.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);
  DrawHLine(370.0f, 420.0f, 180.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);
  DrawHLine(465.0f, 530.0f, 180.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);

  // Lines for third row (OP CODE and MEM ADDR)
  DrawHLine(130.0f, 235.0f, 270.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);
  DrawHLine(400.0f, 420.0f, 270.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);
  DrawHLine(465.0f, 550.0f, 270.0f, scale, layout_origin, display_line_color, 1.0f, draw_list);

  for (size_t i = 0; i < PanelLayout::kLampStrips.size(); ++i) {
    if (i >= 6) {
      break;
    }
    DrawLampStrip(layout_origin, scale, PanelLayout::kLampStrips[i], lamp_values[i], lamp_test,
                  draw_list);
  }

  LampStripLayout countdown = PanelLayout::kLampStrips[6];
  LampStripLayout distributor = PanelLayout::kLampStrips[7];
  LampStripLayout prog_addr = PanelLayout::kLampStrips[8];
  float bottom_y = panel_height_units - 40.0f;
  countdown.y = static_cast<int>(bottom_y);
  distributor.y = static_cast<int>(bottom_y);
  prog_addr.y = static_cast<int>(bottom_y);

  float w1 = strip_width(countdown);
  float w2 = strip_width(distributor);
  float w3 = strip_width(prog_addr);
  float total = w1 + w2 + w3;
  float spacing = (panel_width_units - total) / 4.0f;
  if (spacing < 10.0f) {
    spacing = 10.0f;
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
  ButtonStyle keypad_style{Color(235, 230, 220), Color(245, 240, 230), Color(215, 210, 200),
                           Color(30, 30, 30)};
  // Default control button style
  ButtonStyle control_style{Color(180, 175, 165), Color(200, 195, 185), Color(160, 155, 145),
                            Color(30, 30, 30)};
  // Colored control buttons matching historical panel
  ButtonStyle clear_style{Color(220, 200, 80), Color(240, 220, 100), Color(190, 170, 60),
                          Color(30, 30, 30)};
  ButtonStyle stop_style{Color(200, 80, 80), Color(220, 100, 100), Color(170, 60, 60),
                         Color(255, 255, 255)};
  ButtonStyle start_style{Color(80, 160, 80), Color(100, 180, 100), Color(60, 140, 60),
                          Color(255, 255, 255)};
  // Toggle switches with darker appearance
  ToggleStyle toggle_style{Color(140, 135, 125), Color(160, 150, 130), Color(80, 75, 70),
                           Color(40, 40, 40), Color(30, 30, 30)};
  // Dark text color for cream background
  ImU32 dark_text = Color(40, 35, 30);
  // Line color for decorations
  ImU32 line_color = Color(60, 55, 50);

  // === LEFT SIDE: POWER, COM-TRAN TEN, LAMP TEST ===

  // POWER - label above switch
  DrawStackedText(88.0f, 145.0f, "POWER", nullptr, scale, origin, dark_text, draw_list);
  if (DrawToggleSwitch("power_switch", PanelLayout::kPower.rect, scale, origin,
                       state.panel_input.power_on, toggle_style, draw_list)) {
    state.panel_input.power_on = !state.panel_input.power_on;
  }

  // LAMP TEST - stacked text above button
  DrawStackedText(90.0f, 340.0f, "LAMP", "TEST", scale, origin, dark_text, draw_list);
  state.panel_input.lamp_test = DrawUnlabeledHoldButton("lamp_test",
                                                        PanelLayout::kLampTest.rect, scale, origin,
                                                        control_style, draw_list);
  bool lamp_test = state.panel_input.power_on && state.panel_input.lamp_test;

  // === HEX KEYPAD ===
  for (const auto& key : PanelLayout::kHexKeypad) {
    char label[2] = {key.label, 0};
    char id[16];
    std::snprintf(id, sizeof(id), "key_%c", key.label);
    if (DrawMomentaryButton(id, label, key.rect, scale, origin, keypad_style, draw_list)) {
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
  DrawHLine(180.0f, 310.0f, 35.0f, scale, origin, line_color, 1.5f, draw_list);
  DrawGroupLabel(PanelLayout::kInputLabel, scale, origin, draw_list, dark_text);
  DrawHLine(400.0f, 550.0f, 35.0f, scale, origin, line_color, 1.5f, draw_list);

  for (size_t i = 0; i < PanelLayout::kInputToggles.size(); ++i) {
    const auto& toggle = PanelLayout::kInputToggles[i];
    bool on = (state.panel_input.input_switches >> (9 - i)) & 0x1u;
    bool lit = lamp_test || on;
    if (DrawInputLampButton(toggle, scale, origin, lit, draw_list)) {
      uint16_t mask = static_cast<uint16_t>(1u << (9 - i));
      state.panel_input.input_switches ^= mask;
    }
  }

  // === RESET button ===
  DrawLabelAbove("RESET", PanelLayout::kReset.rect, 16.0f, scale, origin, dark_text, draw_list);
  if (DrawUnlabeledButton("reset", PanelLayout::kReset.rect, scale, origin, control_style,
                          draw_list)) {
    state.panel_input.reset = true;
  }

  // === I/O MODE section (Row 1, left) ===
  DrawHLine(540.0f, 570.0f, 110.0f, scale, origin, line_color, 1.5f, draw_list);
  DrawGroupLabel(PanelLayout::kIoModeLabel, scale, origin, draw_list, dark_text);
  DrawHLine(700.0f, 750.0f, 110.0f, scale, origin, line_color, 1.5f, draw_list);
  for (size_t i = 0; i < PanelLayout::kIoModeToggles.size(); ++i) {
    bool on = state.panel_input.io_mode == i;
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kIoModeToggles[i], scale, origin, lit, toggle_style,
                            draw_list)) {
      state.panel_input.io_mode = static_cast<uint8_t>(i);
    }
  }

  // === MODE section (Row 2, left) ===
  DrawHLine(540.0f, 585.0f, 190.0f, scale, origin, line_color, 1.5f, draw_list);
  DrawGroupLabel(PanelLayout::kModeLabel, scale, origin, draw_list, dark_text);
  DrawHLine(660.0f, 750.0f, 190.0f, scale, origin, line_color, 1.5f, draw_list);
  for (size_t i = 0; i < PanelLayout::kModeToggles.size(); ++i) {
    bool on = state.panel_input.mode == i;
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kModeToggles[i], scale, origin, lit, toggle_style,
                            draw_list)) {
      state.panel_input.mode = static_cast<uint8_t>(i);
    }
  }

  // RPT and SINGLE
  for (size_t i = 0; i < PanelLayout::kRptSenseToggles.size(); ++i) {
    bool on = (i == 0) ? state.panel_input.rpt : state.panel_input.sense;
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kRptSenseToggles[i], scale, origin, lit, toggle_style,
                            draw_list)) {
      if (i == 0) {
        state.panel_input.rpt = !state.panel_input.rpt;
      } else {
        state.panel_input.sense = !state.panel_input.sense;
      }
    }
  }

  // === ERROR BYPASS section (Row 3, left) ===
  DrawHLine(480.0f, 530.0f, 280.0f, scale, origin, line_color, 1.5f, draw_list);
  DrawGroupLabel(PanelLayout::kErrorBypassLabel, scale, origin, draw_list, dark_text);
  DrawHLine(680.0f, 750.0f, 280.0f, scale, origin, line_color, 1.5f, draw_list);
  for (size_t i = 0; i < PanelLayout::kErrorBypassToggles.size(); ++i) {
    bool on = false;
    if (i == 0) {
      on = state.panel_input.error_inst;
    } else if (i == 1) {
      on = state.panel_input.error_add;
    } else {
      on = state.panel_input.error_div;
    }
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kErrorBypassToggles[i], scale, origin, lit,
                            toggle_style, draw_list)) {
      if (i == 0) {
        state.panel_input.error_inst = !state.panel_input.error_inst;
      } else if (i == 1) {
        state.panel_input.error_add = !state.panel_input.error_add;
      } else {
        state.panel_input.error_div = !state.panel_input.error_div;
      }
    }
  }

  // === I/O section (READ/WRITE INSTR/BLOCK) ===
  DrawGroupLabel(PanelLayout::kIoLabel, scale, origin, draw_list, dark_text);
  bool read_intrp = state.panel_input.io_read && state.panel_input.io_intrp;
  bool write_block = state.panel_input.io_write && state.panel_input.io_block;

  bool read_lit = lamp_test || read_intrp;
  if (DrawToggleWithLabel(PanelLayout::kIoToggles[0], scale, origin, read_lit,
                          toggle_style, draw_list) ||
      DrawToggleWithLabel(PanelLayout::kIoToggles[2], scale, origin, read_lit,
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
  if (DrawToggleWithLabel(PanelLayout::kIoToggles[1], scale, origin, write_lit,
                          toggle_style, draw_list) ||
      DrawToggleWithLabel(PanelLayout::kIoToggles[3], scale, origin, write_lit,
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
  DrawHLine(765.0f, 795.0f, 110.0f, scale, origin, line_color, 1.5f, draw_list);
  DrawGroupLabel(PanelLayout::kControlLabel, scale, origin, draw_list, dark_text);
  DrawHLine(895.0f, 940.0f, 110.0f, scale, origin, line_color, 1.5f, draw_list);

  // Draw control buttons with labels above and individual colors
  for (const auto& control : PanelLayout::kControls) {
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
    DrawLabelAbove(control.name, control.rect, 18.0f, scale, origin, dark_text, draw_list);
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
  DrawGroupLabel(PanelLayout::kRegisterLoadLabel, scale, origin, draw_list, dark_text);
  for (const auto& load : PanelLayout::kRegisterLoads) {
    char id[32];
    std::snprintf(id, sizeof(id), "load_%s", load.name);
    // Use buttons with internal labels (matching historical panel style)
    if (DrawMomentaryButton(id, load.name, load.rect, scale, origin, control_style, draw_list)) {
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
  DrawGroupLabel(PanelLayout::kMemoryLabel, scale, origin, draw_list, dark_text);
  for (size_t i = 0; i < PanelLayout::kMemoryToggles.size(); ++i) {
    bool on = (i == 0) ? state.panel_input.mem_read : state.panel_input.mem_write;
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kMemoryToggles[i], scale, origin, lit,
                            toggle_style, draw_list)) {
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
  DrawStackedText(88.0f, 260.0f, "COM-TRAN", "TEN", scale, origin, dark_text, draw_list);
  // DIGIAC corporation - top right corner (above RESET)
  draw_list->AddText(ImVec2(origin.x + 840.0f * scale, origin.y + 35.0f * scale),
                     dark_text, "DIGIAC corporation");
}

}  // namespace

void PanelView::Draw(core::MachineState& state) const {
  ImVec2 display = ImGui::GetIO().DisplaySize;
  float right_column = 360.0f;
  float panel_width = std::max(420.0f, display.x - right_column - 30.0f);
  float available_height = std::max(520.0f, display.y - 40.0f);
  float gap = 16.0f;
  float top_height = std::max(240.0f, available_height * 0.42f);
  float bottom_height = std::max(280.0f, available_height - top_height - gap);
  if (top_height + bottom_height + gap > available_height) {
    bottom_height = std::max(200.0f, available_height - top_height - gap);
  }

  float display_x = 20.0f;
  float input_x = 20.0f;
  float display_y = 20.0f;
  float input_y = display_y + top_height + gap;

  ImGui::SetNextWindowPos(ImVec2(input_x, input_y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(panel_width, bottom_height), ImGuiCond_Always);
  ImGui::Begin("Input Panel", nullptr,
               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

  ImVec2 input_origin = ImGui::GetCursorScreenPos();
  ImVec2 input_avail = ImGui::GetContentRegionAvail();
  float input_margin = 14.0f;
  float input_scale = std::min((input_avail.x - 2.0f * input_margin) / PanelLayout::kInputWidth,
                               (input_avail.y - 2.0f * input_margin) / PanelLayout::kInputHeight);
  input_scale = std::max(input_scale, 0.1f);

  ImVec2 input_layout_origin(input_origin.x + input_margin,
                             input_origin.y + input_margin);
  ImDrawList* input_draw_list = ImGui::GetWindowDrawList();
  // Cream/beige metal panel background matching historical panel
  DrawPanelBackground(input_origin, input_avail,
                      Color(215, 205, 190), Color(210, 200, 185),
                      Color(195, 185, 170), Color(200, 190, 175), input_draw_list);

  DrawInputPanel(state, input_layout_origin, input_scale, input_draw_list);

  ImGui::Dummy(input_avail);
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(display_x, display_y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(panel_width, top_height), ImGuiCond_Always);
  ImGui::Begin("Display Panel", nullptr,
               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

  ImVec2 display_origin = ImGui::GetCursorScreenPos();
  ImVec2 display_avail = ImGui::GetContentRegionAvail();
  float margin = 14.0f;
  float display_scale = std::min((display_avail.x - 2.0f * margin) / PanelLayout::kDisplayWidth,
                                 (display_avail.y - 2.0f * margin) / PanelLayout::kDisplayHeight);
  display_scale = std::max(display_scale, 0.1f);

  float display_units_w = display_avail.x / display_scale;
  float display_units_h = display_avail.y / display_scale;
  ImVec2 display_layout_origin(display_origin.x + margin,
                               display_origin.y + margin);
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  // Dark charcoal/slate background matching historical panel
  DrawPanelBackground(display_origin, display_avail,
                      Color(50, 55, 60), Color(45, 50, 55),
                      Color(35, 40, 45), Color(40, 45, 50), draw_list);

  DrawDisplayPanel(state, display_layout_origin, display_origin, display_scale,
                   display_units_w, display_units_h, draw_list);

  ImGui::Dummy(display_avail);
  ImGui::End();
}

}  // namespace ct10::ui
