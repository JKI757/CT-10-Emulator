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

void DrawGroupFrameWithLabelBounds(float label_left,
                                   float label_right,
                                   float group_left,
                                   float group_right,
                                   float line_y,
                                   float bottom_y,
                                   float scale,
                                   const ImVec2& origin,
                                   ImU32 color,
                                   float thickness,
                                   ImDrawList* draw_list,
                                   bool draw_caps);

void DrawGroupFrame(const TextLabel& label,
                    float group_left,
                    float group_right,
                    float line_y,
                    float bottom_y,
                    float scale,
                    const ImVec2& origin,
                    ImU32 color,
                    float thickness,
                    ImDrawList* draw_list) {
  ImVec2 label_size = ImGui::CalcTextSize(label.label);
  float label_left = static_cast<float>(label.x);
  float scale_inv = (scale > 0.0f) ? (1.0f / scale) : 1.0f;
  float label_right = label_left + label_size.x * scale_inv;
  DrawGroupFrameWithLabelBounds(label_left, label_right, group_left, group_right,
                                line_y, bottom_y, scale, origin, color, thickness,
                                draw_list, true);
}

void DrawGroupFrameWithLabelBounds(float label_left,
                                   float label_right,
                                   float group_left,
                                   float group_right,
                                   float line_y,
                                   float bottom_y,
                                   float scale,
                                   const ImVec2& origin,
                                   ImU32 color,
                                   float thickness,
                                   ImDrawList* draw_list,
                                   bool draw_caps) {
  float left_edge = group_left + PanelLayout::kGroupLineEdgePad;
  float right_edge = group_right - PanelLayout::kGroupLineEdgePad;
  float left_end = label_left - PanelLayout::kGroupLineLabelPad;
  float right_start = label_right + PanelLayout::kGroupLineLabelPad;

  if (left_end > left_edge) {
    DrawHLine(left_edge, left_end, line_y, scale, origin, color, thickness, draw_list);
  }
  if (right_edge > right_start) {
    DrawHLine(right_start, right_edge, line_y, scale, origin, color, thickness, draw_list);
  }

  DrawVLine(left_edge, line_y, bottom_y, scale, origin, color, thickness, draw_list);
  DrawVLine(right_edge, line_y, bottom_y, scale, origin, color, thickness, draw_list);
  if (draw_caps && PanelLayout::kGroupLineCapLength > 0.0f) {
    float cap = PanelLayout::kGroupLineCapLength;
    DrawHLine(left_edge, left_edge + cap, bottom_y, scale, origin, color,
              thickness, draw_list);
    DrawHLine(right_edge - cap, right_edge, bottom_y, scale, origin, color,
              thickness, draw_list);
  }
}

float LampStripWidth(const LampStripLayout& layout);
float LampOffsetForIndex(const LampStripLayout& layout, int index);
void DrawDisplayGroupFrameRange(const ImVec2& origin,
                                float scale,
                                const LampStripLayout& layout,
                                float group_left,
                                float group_right,
                                ImU32 color,
                                float thickness,
                                ImDrawList* draw_list);
void DrawLowerLabelFrame(const ImVec2& origin,
                         float scale,
                         const char* label,
                         float label_center_x,
                         float group_left,
                         float group_right,
                         float line_y,
                         float line_length,
                         ImU32 color,
                         float thickness,
                         ImDrawList* draw_list);

void DrawDisplayGroupFrame(const ImVec2& origin,
                           float scale,
                           const LampStripLayout& layout,
                           ImU32 color,
                           float thickness,
                           ImDrawList* draw_list) {
  float strip_width = LampStripWidth(layout);
  float group_left = layout.x;
  float group_right = layout.x + strip_width;
  DrawDisplayGroupFrameRange(origin, scale, layout, group_left, group_right,
                             color, thickness, draw_list);
}

void DrawDisplayGroupFrameRange(const ImVec2& origin,
                                float scale,
                                const LampStripLayout& layout,
                                float group_left,
                                float group_right,
                                ImU32 color,
                                float thickness,
                                ImDrawList* draw_list) {
  float strip_width = LampStripWidth(layout);
  ImVec2 label_size = ImGui::CalcTextSize(layout.label);
  float scale_inv = (scale > 0.0f) ? (1.0f / scale) : 1.0f;
  float label_width = label_size.x * scale_inv;
  float label_left =
      layout.x + (strip_width - label_width) * 0.5f + layout.label_offset_x;
  float label_right = label_left + label_width;
  float line_y = layout.y + layout.label_offset_y;
  if (layout.label_offset_y < 0.0f) {
    line_y += PanelLayout::kGroupLineYOffset;
  } else {
    line_y -= PanelLayout::kGroupLineYOffset;
  }
  float bottom_y = line_y + PanelLayout::kLampSize * PanelLayout::kGroupLineDropFactor +
                   PanelLayout::kDisplayLineDropExtra;
  DrawGroupFrameWithLabelBounds(label_left, label_right, group_left,
                                group_right, line_y, bottom_y,
                                scale, origin, color, thickness, draw_list, false);
}

void DrawLowerLabelFrame(const ImVec2& origin,
                         float scale,
                         const char* label,
                         float label_center_x,
                         float group_left,
                         float group_right,
                         float line_y,
                         float line_length,
                         ImU32 color,
                         float thickness,
                         ImDrawList* draw_list) {
  ImVec2 label_size = ImGui::CalcTextSize(label);
  float scale_inv = (scale > 0.0f) ? (1.0f / scale) : 1.0f;
  float label_half = label_size.x * scale_inv * 0.5f;
  float label_left = label_center_x - label_half;
  float label_right = label_center_x + label_half;
  float bottom_y = line_y - line_length;
  DrawGroupFrameWithLabelBounds(label_left, label_right, group_left,
                                group_right, line_y, bottom_y,
                                scale, origin, color, thickness, draw_list, false);
}

void DrawIndicatorGroupFrame(const ImVec2& origin,
                             float scale,
                             const TextLabel& label,
                             int base_x,
                             int base_y,
                             int count,
                             float label_center_x,
                             float line_x,
                             ImU32 color,
                             float thickness,
                             ImDrawList* draw_list) {
  if (count <= 0) {
    return;
  }
  float line_height = ImGui::GetTextLineHeight();
  float label_y = static_cast<float>(label.y) + line_height * 0.5f;
  float top_y = static_cast<float>(base_y) + PanelLayout::kGroupLineEdgePad;
  float bottom_y =
      static_cast<float>(base_y) +
      (count - 1) * PanelLayout::kIndicatorStepY +
      PanelLayout::kLampSize - PanelLayout::kGroupLineEdgePad;

  float line_start = label_center_x + PanelLayout::kIndicatorLineLabelCenterOffsetX;
  if (line_x > line_start) {
    DrawHLine(line_start, line_x, label_y, scale, origin,
              color, thickness, draw_list);
  }
  DrawVLine(line_x, top_y, bottom_y, scale, origin, color, thickness, draw_list);
  if (PanelLayout::kGroupLineCapLength > 0.0f) {
    float cap = PanelLayout::kGroupLineCapLength;
    DrawHLine(line_x, line_x + cap, top_y, scale, origin, color, thickness, draw_list);
    DrawHLine(line_x, line_x + cap, bottom_y, scale, origin, color, thickness,
              draw_list);
  }
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

float LampStripWidth(const LampStripLayout& layout) {
  float lamp = PanelLayout::kLampSize;
  float gap = PanelLayout::kLampGap;
  float group_gap = PanelLayout::kLampGroupGap;
  if (layout.bits <= 0) {
    return 0.0f;
  }
  float width = layout.bits * (lamp + gap) - gap;
  if (layout.gap_after0 >= 0) {
    width += group_gap * layout.gap_scale0;
  }
  if (layout.gap_after1 >= 0) {
    width += group_gap * layout.gap_scale1;
  }
  return width;
}

float LampOffsetForIndex(const LampStripLayout& layout, int index) {
  float lamp = PanelLayout::kLampSize;
  float gap = PanelLayout::kLampGap;
  float group_gap = PanelLayout::kLampGroupGap;
  float offset = index * (lamp + gap);
  if (layout.gap_after0 >= 0 && index >= layout.gap_after0) {
    offset += group_gap * layout.gap_scale0;
  }
  if (layout.gap_after1 >= 0 && index >= layout.gap_after1) {
    offset += group_gap * layout.gap_scale1;
  }
  return offset;
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

  float strip_width = LampStripWidth(layout);
  ImVec2 label_size = ImGui::CalcTextSize(layout.label);
  float scale_inv = (scale > 0.0f) ? (1.0f / scale) : 1.0f;
  float label_width = label_size.x * scale_inv;
  float label_x =
      layout.x + (strip_width - label_width) * 0.5f + layout.label_offset_x;
  float label_y = layout.y + layout.label_offset_y;
  ImVec2 label_pos(origin.x + label_x * scale, origin.y + label_y * scale);
  draw_list->AddText(label_pos, Color(PanelLayout::kTextLight), layout.label);

  int bits = layout.bits;
  for (int i = 0; i < bits; ++i) {
    int bit = (bits - 1) - i + layout.bit_offset;
    float offset = LampOffsetForIndex(layout, i);
    float x = layout.x + offset;
    float y = layout.y;
    bool on = lamp_test ? true : ((value >> bit) & 0x1u);
    DrawLampBox(origin, scale, x, y, lamp, on, draw_list);

    if (!layout.show_bit_labels) {
      continue;
    }
    const char* bit_label_text = nullptr;
    char bit_label[4];
    if (layout.bit_labels) {
      bit_label_text = layout.bit_labels[i];
    } else {
      std::snprintf(bit_label, sizeof(bit_label), "%d", bit);
      bit_label_text = bit_label;
    }
    ImVec2 text_size = ImGui::CalcTextSize(bit_label_text);
    float text_x = origin.x + (x + (lamp - text_size.x) * 0.5f) * scale;
    float text_y = origin.y + (y + (lamp - text_size.y) * 0.5f) * scale;
    draw_list->AddText(ImVec2(text_x, text_y), Color(PanelLayout::kTextMuted),
                       bit_label_text);
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
  ImU32 display_line_color = Color(PanelLayout::kDisplayLineColor);
  float scale_inv = (scale > 0.0f) ? (1.0f / scale) : 1.0f;
  auto label_center_x = [&](const TextLabel& label) {
    ImVec2 size = ImGui::CalcTextSize(label.label);
    return static_cast<float>(label.x) + size.x * scale_inv * 0.5f;
  };
  float indicator_label_center_x =
      (label_center_x(PanelLayout::kCondLabel) +
       label_center_x(PanelLayout::kStatusLabel) +
       label_center_x(PanelLayout::kErrorLabel)) / 3.0f;
  float indicator_line_x =
      static_cast<float>(PanelLayout::kCondGroupBaseX) +
      PanelLayout::kIndicatorLineOffsetX;

  DrawIndicatorGroupFrame(layout_origin, scale, PanelLayout::kCondLabel,
                          PanelLayout::kCondGroupBaseX,
                          PanelLayout::kCondGroupBaseY,
                          PanelLayout::kIndicatorCondCount,
                          indicator_label_center_x,
                          indicator_line_x,
                          display_line_color,
                          PanelLayout::kDisplayLineThickness,
                          draw_list);
  DrawIndicatorGroupFrame(layout_origin, scale, PanelLayout::kStatusLabel,
                          PanelLayout::kStatusGroupBaseX,
                          PanelLayout::kStatusGroupBaseY,
                          PanelLayout::kIndicatorStatusCount,
                          indicator_label_center_x,
                          indicator_line_x,
                          display_line_color,
                          PanelLayout::kDisplayLineThickness,
                          draw_list);
  DrawIndicatorGroupFrame(layout_origin, scale, PanelLayout::kErrorLabel,
                          PanelLayout::kErrorGroupBaseX,
                          PanelLayout::kErrorGroupBaseY,
                          PanelLayout::kIndicatorErrorCount,
                          indicator_label_center_x,
                          indicator_line_x,
                          display_line_color,
                          PanelLayout::kDisplayLineThickness,
                          draw_list);
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

  bool any_error = power_on &&
                   (state.flags.inst_error || state.flags.add_overflow ||
                    state.flags.divide_overflow);
  uint16_t distributor_value =
      static_cast<uint16_t>(power_on ? state.distributor.value() : 0);
  if (any_error) {
    distributor_value |= 0x10;
  }
  const uint16_t lamp_values[] = {
      static_cast<uint16_t>(power_on ? state.accumulator.value() : 0),
      static_cast<uint16_t>(power_on ? state.quotient.value() : 0),
      static_cast<uint16_t>(power_on ? state.buffer.value() : 0),
      static_cast<uint16_t>(power_on ? state.index.value() : 0),
      static_cast<uint16_t>(power_on ? state.opcode.value() : 0),
      static_cast<uint16_t>(power_on ? state.mar.value() : 0),
      static_cast<uint16_t>(power_on ? state.countdown.value() : 0),
      distributor_value,
      static_cast<uint16_t>(power_on ? state.par.value() : 0),
  };

  auto lamp_left = [&](const LampStripLayout& layout, int index) {
    return layout.x + LampOffsetForIndex(layout, index);
  };
  auto lamp_right = [&](const LampStripLayout& layout, int index) {
    return lamp_left(layout, index) + PanelLayout::kLampSize;
  };

  // Decorative lines for display panel (group-relative frames)
  for (size_t i = 0; i < PanelLayout::kTopLampStripCount; ++i) {
    if (i == static_cast<size_t>(PanelLayout::kOpCodeStripIndex) ||
        i == static_cast<size_t>(PanelLayout::kOperandStripIndex)) {
      continue;
    }
    DrawDisplayGroupFrame(layout_origin, scale,
                          PanelLayout::kLampStrips[i],
                          display_line_color,
                          PanelLayout::kDisplayLineThickness,
                          draw_list);
  }

  const LampStripLayout& op_strip =
      PanelLayout::kLampStrips[PanelLayout::kOpCodeStripIndex];
  const LampStripLayout& operand_strip =
      PanelLayout::kLampStrips[PanelLayout::kOperandStripIndex];
  float op_width = LampStripWidth(op_strip);
  float operand_width = LampStripWidth(operand_strip);
  float mem_bit8_right = lamp_right(operand_strip, 1);
  float mem_bit7_left = lamp_left(operand_strip, 2);
  float mem_bit0_right = lamp_right(operand_strip, operand_strip.bits - 1);

  DrawDisplayGroupFrameRange(layout_origin, scale, op_strip,
                             op_strip.x, mem_bit8_right,
                             display_line_color,
                             PanelLayout::kDisplayLineThickness,
                             draw_list);
  DrawDisplayGroupFrameRange(layout_origin, scale, operand_strip,
                             mem_bit7_left, mem_bit0_right,
                             display_line_color,
                             PanelLayout::kDisplayLineThickness,
                             draw_list);
  float op_label_y = op_strip.y + PanelLayout::kDisplayOpSubLabelOffsetY;
  float op_line_end =
      lamp_left(op_strip, op_strip.bits - 1) - PanelLayout::kDisplayOpSubLinePadX;
  float op_frame_right = op_line_end + PanelLayout::kGroupLineEdgePad;
  float op_label_x =
      (op_strip.x + op_frame_right) * 0.5f + PanelLayout::kDisplayOpCodeLowerLabelOffsetX;
  float op_right_center_x =
      op_strip.x + LampOffsetForIndex(op_strip, op_strip.bits - 1) +
      PanelLayout::kLampSize * 0.5f;
  float x_label_x = op_right_center_x + PanelLayout::kDisplayXLowerLabelOffsetX;
  float mem_label_x =
      (operand_strip.x + mem_bit0_right) * 0.5f +
      PanelLayout::kDisplayMemAddrLowerLabelOffsetX;

  for (size_t i = 0; i < PanelLayout::kLampStrips.size(); ++i) {
    if (i >= static_cast<size_t>(PanelLayout::kTopLampStripCount)) {
      break;
    }
    DrawLampStrip(layout_origin, scale, PanelLayout::kLampStrips[i], lamp_values[i], lamp_test,
                  draw_list);
  }

  float line_height = ImGui::GetTextLineHeight();
  float op_sub_line_y =
      op_label_y + line_height * 0.5f + PanelLayout::kDisplayOpSubLineOffsetY;
  float line_length =
      PanelLayout::kLampSize * PanelLayout::kGroupLineDropFactor +
      PanelLayout::kDisplayLineDropExtra;
  DrawLowerLabelFrame(layout_origin, scale,
                      PanelLayout::kDisplayOpCodeLowerLabelText,
                      op_label_x, op_strip.x, op_frame_right,
                      op_sub_line_y, line_length,
                      display_line_color, PanelLayout::kDisplayLineThickness, draw_list);
  DrawLowerLabelFrame(layout_origin, scale,
                      PanelLayout::kDisplayMemAddrLowerLabelText,
                      mem_label_x, operand_strip.x,
                      mem_bit0_right,
                      op_sub_line_y, line_length,
                      display_line_color, PanelLayout::kDisplayLineThickness, draw_list);
  DrawVLine(x_label_x, op_sub_line_y,
            op_strip.y + PanelLayout::kLampSize - PanelLayout::kDisplayXLinePadY,
            scale, layout_origin, display_line_color,
            PanelLayout::kDisplayLineThickness, draw_list);

  auto draw_centered = [&](const char* text, float x, float y) {
    ImVec2 size = ImGui::CalcTextSize(text);
    ImVec2 pos(layout_origin.x + x * scale - size.x * 0.5f,
               layout_origin.y + y * scale);
    draw_list->AddText(pos, Color(PanelLayout::kTextLight), text);
  };
  draw_centered(PanelLayout::kDisplayOpCodeLowerLabelText, op_label_x, op_label_y);
  draw_centered(PanelLayout::kDisplayXLowerLabelText, x_label_x, op_label_y);
  draw_centered(PanelLayout::kDisplayMemAddrLowerLabelText, mem_label_x, op_label_y);

  LampStripLayout countdown = PanelLayout::kLampStrips[PanelLayout::kCountdownStripIndex];
  LampStripLayout distributor = PanelLayout::kLampStrips[PanelLayout::kDistributorStripIndex];
  LampStripLayout prog_addr = PanelLayout::kLampStrips[PanelLayout::kProgramAddressStripIndex];
  float bottom_y = panel_height_units - PanelLayout::kDisplayBottomMargin -
                   PanelLayout::kDisplayLabelBelowOffsetY +
                   PanelLayout::kDisplayBottomRowOffsetY;
  countdown.y = bottom_y;
  distributor.y = bottom_y;
  prog_addr.y = bottom_y;

  float w1 = LampStripWidth(countdown);
  float w2 = LampStripWidth(distributor);
  float w3 = LampStripWidth(prog_addr);
  float x1 = PanelLayout::kDisplayBottomGroupBaseX;
  float x2 = x1 + w1 + PanelLayout::kDisplayBottomGroupGap;
  float x3 = x2 + w2 + PanelLayout::kDisplayBottomGroupGap;
  float margin_units = (layout_origin.x - panel_origin.x) / scale;
  float operand_right = operand_strip.x + operand_width + margin_units;
  float bottom_right = x3 + w3;
  float bottom_shift =
      operand_right - bottom_right + PanelLayout::kDisplayBottomAlignOffsetX;
  x1 += bottom_shift;
  x2 += bottom_shift;
  x3 += bottom_shift;
  countdown.x = x1;
  distributor.x = x2;
  prog_addr.x = x3;

  auto bottom_label_center = [&](const LampStripLayout& layout) {
    float strip_width = LampStripWidth(layout);
    return layout.x + strip_width * 0.5f + layout.label_offset_x;
  };
  float bottom_line_length =
      PanelLayout::kLampSize * PanelLayout::kGroupLineDropFactor +
      PanelLayout::kDisplayLineDropExtra;
  float bottom_line_height = ImGui::GetTextLineHeight();
  float bottom_countdown_line_y =
      countdown.y + countdown.label_offset_y + bottom_line_height * 0.5f +
      PanelLayout::kDisplayBottomLineOffsetY;
  float bottom_distributor_line_y =
      distributor.y + distributor.label_offset_y + bottom_line_height * 0.5f +
      PanelLayout::kDisplayBottomLineOffsetY;
  float bottom_program_line_y =
      prog_addr.y + prog_addr.label_offset_y + bottom_line_height * 0.5f +
      PanelLayout::kDisplayBottomLineOffsetY;

  DrawLowerLabelFrame(panel_origin, scale, countdown.label,
                      bottom_label_center(countdown),
                      countdown.x, countdown.x + w1,
                      bottom_countdown_line_y, bottom_line_length,
                      display_line_color, PanelLayout::kDisplayLineThickness, draw_list);
  DrawLowerLabelFrame(panel_origin, scale, distributor.label,
                      bottom_label_center(distributor),
                      distributor.x, distributor.x + w2,
                      bottom_distributor_line_y, bottom_line_length,
                      display_line_color, PanelLayout::kDisplayLineThickness, draw_list);
  DrawLowerLabelFrame(panel_origin, scale, prog_addr.label,
                      bottom_label_center(prog_addr),
                      prog_addr.x, prog_addr.x + w3,
                      bottom_program_line_y, bottom_line_length,
                      display_line_color, PanelLayout::kDisplayLineThickness, draw_list);

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
  DrawGroupFrame(PanelLayout::kInputGroup.label,
                 PanelLayout::kInputGroupLeftX,
                 PanelLayout::kInputGroupRightX,
                 PanelLayout::kInputLineY,
                 PanelLayout::kInputLineBottomY,
                 scale, origin, line_color,
                 PanelLayout::kInputLineThickness, draw_list);
  DrawGroupLabel(PanelLayout::kInputGroup.label, scale, origin, draw_list, dark_text);

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
  DrawGroupFrame(PanelLayout::kIoModeGroup.label,
                 PanelLayout::kIoModeGroupLeftX,
                 PanelLayout::kIoModeGroupRightX,
                 PanelLayout::kIoModeLineY,
                 PanelLayout::kIoModeLineBottomY,
                 scale, origin, line_color,
                 PanelLayout::kSectionLineThickness, draw_list);
  DrawGroupLabel(PanelLayout::kIoModeGroup.label, scale, origin, draw_list, dark_text);
  for (size_t i = 0; i < PanelLayout::kIoModeGroup.toggles.size(); ++i) {
    bool on = state.panel_input.io_mode == i;
    bool lit = lamp_test || on;
    if (DrawToggleWithLabel(PanelLayout::kIoModeGroup.toggles[i], scale, origin, lit,
                            PanelLayout::kIoModeGroup.label_offset, toggle_style, draw_list)) {
      state.panel_input.io_mode = static_cast<uint8_t>(i);
    }
  }

  // === MODE section (Row 2, includes RPT/SINGLE) ===
  DrawGroupFrame(PanelLayout::kModeGroup.label,
                 PanelLayout::kModeGroupLeftX,
                 PanelLayout::kModeGroupRightX,
                 PanelLayout::kModeLineY,
                 PanelLayout::kModeLineBottomY,
                 scale, origin, line_color,
                 PanelLayout::kSectionLineThickness, draw_list);
  DrawGroupLabel(PanelLayout::kModeGroup.label, scale, origin, draw_list, dark_text);
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
  DrawGroupFrame(PanelLayout::kErrorBypassGroup.label,
                 PanelLayout::kErrorBypassGroupLeftX,
                 PanelLayout::kErrorBypassGroupRightX,
                 PanelLayout::kErrorBypassLineY,
                 PanelLayout::kErrorBypassLineBottomY,
                 scale, origin, line_color,
                 PanelLayout::kSectionLineThickness, draw_list);
  DrawGroupLabel(PanelLayout::kErrorBypassGroup.label, scale, origin, draw_list, dark_text);
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
  DrawGroupFrame(PanelLayout::kIoGroup.label,
                 PanelLayout::kIoGroupLeftX,
                 PanelLayout::kIoGroupRightX,
                 PanelLayout::kIoLineY,
                 PanelLayout::kIoLineBottomY,
                 scale, origin, line_color,
                 PanelLayout::kSectionLineThickness, draw_list);
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
  DrawGroupFrame(PanelLayout::kControlGroup.label,
                 PanelLayout::kControlGroupLeftX,
                 PanelLayout::kControlGroupRightX,
                 PanelLayout::kControlLineY,
                 PanelLayout::kControlLineBottomY,
                 scale, origin, line_color,
                 PanelLayout::kSectionLineThickness, draw_list);
  DrawGroupLabel(PanelLayout::kControlGroup.label, scale, origin, draw_list, dark_text);

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
  if (PanelLayout::kInputPanelHeightTrim > 0.0f &&
      bottom_height > PanelLayout::kBottomHeightMin) {
    float trim = std::min(PanelLayout::kInputPanelHeightTrim,
                          bottom_height - PanelLayout::kBottomHeightMin);
    bottom_height -= trim;
    top_height = available_height - bottom_height - gap;
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
