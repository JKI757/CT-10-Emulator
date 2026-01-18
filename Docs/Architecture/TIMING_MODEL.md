# CT-10 Emulator – Timing Model

## Two-Level Timing System

The CT-10 uses hierarchical timing:

1. Distributor pulses (major steps)
2. Clock pulses (minor steps)

---

## Distributor Register (D)

- 4-bit up-counter
- Counts 0–15
- Each count defines a major phase

Each distributor pulse:
- Duration: ~6 µs (scaled in emulator)
- Contains 3 clock pulses

---

## Clock Pulses

Within each distributor pulse:

| Pulse | Purpose |
|------|--------|
| CP1 | Source enable |
| CP2 | Bus transfer |
| CP3 | Destination latch |

---

## Instruction Phases

### Acquisition Phase
- Fetch instruction word(s)
- Load OP Code register
- Load Memory Address Register

### Execution Phase
- Perform instruction-specific micro-ops
- Update flags
- Advance Program Address

---

## Emulator Scaling

Speed multiplier affects:
- Wall-clock delay between pulses

Speed multiplier does NOT affect:
- Ordering
- State transitions

---

## Single-Step Modes

Supported stepping:
- Distributor pulse
- Clock pulse

---

**End of Timing Model**
