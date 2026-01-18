# CT-10 Emulator – Per-Instruction Timing Tables

## Purpose

This document defines **cycle-accurate timing** for CT-10 instructions.
Each instruction is decomposed into Distributor pulses and Clock phases.

Notation:
- Dn = Distributor count n
- CP1 / CP2 / CP3 = Clock pulses within a distributor pulse
- → = bus transfer
- := = register latch

---

## Common Acquisition Phase (All Instructions)

| Dist | CP | Operation |
|----|----|-----------|
| D0 | CP1 | PAR → MAR |
| D0 | CP2 | Memory[MAR] → Z Bus |
| D0 | CP3 | Z Bus → Buffer |
| D1 | CP1 | Buffer → OP Code Register |
| D1 | CP2 | PAR + 1 → PAR |
| D1 | CP3 | (Idle / settle) |

---

## LDA m,x (Load Accumulator)

### Execution Phase

| Dist | CP | Operation |
|----|----|-----------|
| D2 | CP1 | MAR + Index → MAR |
| D2 | CP2 | Memory[MAR] → Z Bus |
| D2 | CP3 | Z Bus → Buffer |
| D3 | CP1 | Buffer → F Bus |
| D3 | CP2 | F Bus → Accumulator |
| D3 | CP3 | Update Condition Flags |

---

## ADD m,x

| Dist | CP | Operation |
|----|----|-----------|
| D2 | CP1 | MAR + Index → MAR |
| D2 | CP2 | Memory[MAR] → Z Bus |
| D2 | CP3 | Z Bus → Buffer |
| D3 | CP1 | Accumulator → Y Bus |
| D3 | CP2 | Buffer → X Bus |
| D3 | CP3 | ALU(Add) → F Bus |
| D4 | CP1 | F Bus → Accumulator |
| D4 | CP2 | Update Carry / Overflow |
| D4 | CP3 | Update Zero / Sign |

---

## DIV m,x

| Dist | CP | Operation |
|----|----|-----------|
| D2 | CP1 | MAR + Index → MAR |
| D2 | CP2 | Memory[MAR] → Z Bus |
| D2 | CP3 | Z Bus → Buffer |
| D3 | CP1 | AQ → ALU Dividend |
| D3 | CP2 | Buffer → ALU Divisor |
| D3 | CP3 | ALU → Q (Quotient) |
| D4 | CP1 | ALU → A (Remainder) |
| D4 | CP2 | Update Divide Overflow |
| D4 | CP3 | Update Condition Flags |

---

## Notes

- No instruction may update registers outside CP3 unless documented
- Distributor reset occurs after execution phase
- Program flow changes occur only after execution completes

---

**End of Timing Tables**
