# CT-10 Emulator – Panel Coordinate Map

## Purpose

Defines spatial layout for a pixel-faithful Dear ImGui front panel.

Coordinate system:
- Origin (0,0) = top-left of panel texture
- Units = pixels

---

## Panel Dimensions

- Width: 1280 px
- Height: 720 px

---

## Hex Keypad

| Key | X | Y | W | H |
|----|----|----|----|----|
| 0 | 820 | 420 | 48 | 48 |
| 1 | 820 | 370 | 48 | 48 |
| 2 | 870 | 370 | 48 | 48 |
| 3 | 920 | 370 | 48 | 48 |
| 4 | 820 | 320 | 48 | 48 |
| 5 | 870 | 320 | 48 | 48 |
| 6 | 920 | 320 | 48 | 48 |
| 7 | 970 | 320 | 48 | 48 |
| 8 | 820 | 270 | 48 | 48 |
| 9 | 870 | 270 | 48 | 48 |
| A | 920 | 270 | 48 | 48 |
| B | 970 | 270 | 48 | 48 |
| C | 820 | 220 | 48 | 48 |
| D | 870 | 220 | 48 | 48 |
| E | 920 | 220 | 48 | 48 |
| F | 970 | 220 | 48 | 48 |

---

## Control Switches

| Control | X | Y | W | H |
|-------|----|----|----|----|
| START | 1040 | 420 | 72 | 36 |
| CLEAR | 1040 | 470 | 72 | 36 |
| LAMP TEST | 1040 | 520 | 72 | 36 |

---

## Register Lamp Rows

Example: Accumulator Lamps

| Bit | X | Y |
|----|----|----|
| 0 | 120 | 180 |
| 1 | 150 | 180 |
| 2 | 180 | 180 |
| 3 | 210 | 180 |
| 4 | 240 | 180 |
| 5 | 270 | 180 |
| 6 | 300 | 180 |
| 7 | 330 | 180 |

---

## Notes

- Coordinates may be fine-tuned once reference photos are overlaid
- Lamps should be rendered as independent draw calls
- Hitboxes must not overlap

---

**End of Panel Map**
