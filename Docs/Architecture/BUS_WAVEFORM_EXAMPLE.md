# CT-10 Emulator – Bus-Level Waveform Example

## Purpose

Provide a concrete, bus-first trace example for validating the simulator’s event ordering.
This is designed to be used as a “known good” reference when bringing up the timing engine,
bus model, and register latching rules.

This is **not** a complete schematic-derived waveform pack. It is a high-value example
to anchor implementation and debugging.

---

## Example Instruction: `LDA m,x`

Scenario:
- Memory[0x010] = 0x20   (OP code = LDA page 0)
- Memory[0x011] = 0x30   (operand address low byte = 0x30)
- PAR = 0x010
- X = 0x00
- Memory[0x030] = 0x27

Goal:
- After execution, A = 0x27

---

## Phase 1: Acquisition (Fetch opcode & operand)

### D0 / CP1: PAR → MAR
- PAR drives Z (address path) → MAR latch
- MAR becomes 0x010

Expected:
- MAR = 0x010
- Buses:
  - Z bus: address bits (implementation-defined)
  - Y/F: idle

### D0 / CP2: Memory[MAR] → Z
- Memory reads 0x20
- Z bus carries complemented data (Z-bus is NOT-form in the hardware model)

Expected:
- Z bus = ~0x20 (bitwise)
- Memory output stable during CP2 window

### D0 / CP3: Z → Buffer
- Buffer latches data from Z during CP3
- Buffer holds true (non-complemented) value

Expected:
- B = 0x20

### D1 / CP1: Buffer → OP Code Register
- B drives Y bus (static selector state)
- OP Code register latches on its defined timing

Expected:
- OP Code register = 0x20

### D1 / CP2: PAR + 1 → PAR
- PAR increments to 0x011

Expected:
- PAR = 0x011

### D1 / CP3: (Set up for operand fetch)
- No external change required, but state should be stable

---

## Operand Fetch

### D2 / CP1: PAR → MAR
Expected:
- MAR = 0x011

### D2 / CP2: Memory[MAR] → Z
Memory reads 0x30

Expected:
- Z bus = ~0x30

### D2 / CP3: Z → Buffer
Expected:
- B = 0x30

### D3 / CP1: Buffer → MAR (lower 8 bits), OP Code bits → MAR (upper 2 bits)
Expected:
- MAR = 0x030 (page derived from opcode, low byte from B)

### D3 / CP2: (Index add if enabled)
- If X indexing active, MAR := MAR + X
- Here X = 0, so MAR unchanged

Expected:
- MAR = 0x030

---

## Phase 2: Execution (Perform load)

### D0 / CP2: Memory[MAR] → Z
Memory reads 0x27

Expected:
- Z bus = ~0x27

### D0 / CP3: Z → Buffer
Expected:
- B = 0x27

### D1 / CP1: Buffer → Y
Expected:
- Y bus = 0x27

### D1 / CP2: Y → ALU → F
With no ALU control asserted, ALU passes B through.

Expected:
- F bus = 0x27

### D1 / CP3: F → Accumulator
Expected:
- A = 0x27

### D15: Update condition flags
Expected:
- Z flag = 0
- GT/LT per sign bit of A

---

## Validation Checklist

- Z bus is complemented when driven by memory/register sources
- Buffer stores true value after Z→B latch
- ALU “no-op passthrough” path works
- A updates only at its defined latch point (not earlier)
- Flags update only at DP15 (or documented equivalent)

---

**End of Example**
