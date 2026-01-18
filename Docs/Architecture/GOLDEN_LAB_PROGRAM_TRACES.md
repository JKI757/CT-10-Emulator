# CT-10 Emulator – Golden Lab Program & Expected Traces

## Purpose

Provide a small canonical program from the training material with deterministic expected results.
This is used for regression tests and as a reference trace during emulator bring-up.

This document includes:
- Memory image
- Expected register values after each instruction
- Expected final memory results

---

## Golden Program: `y = 2x + 5 - z`

As taught in the CT-10 programming section.

Memory map:
- x stored at 0x20
- y stored at 0x21
- z stored at 0x22
- constant 5 stored at 0x23

Program starts at 0x00 and is straight-line.

### Symbolic Program

1. `LDA 20`   ; Load x
2. `ADD 20`   ; Add x (2x)
3. `ADD 23`   ; Add 5
4. `SUB 22`   ; Subtract z
5. `STA 21`   ; Store y
6. `BST 00`   ; Stop

---

## Machine Code Image

All addresses and bytes are hex.

### Program (0x00..0x0B)

| Addr | Byte | Meaning |
|------|------|---------|
| 00 | 20 | LDA opcode |
| 01 | 20 | operand = 0x20 |
| 02 | 60 | ADD opcode |
| 03 | 20 | operand = 0x20 |
| 04 | 60 | ADD opcode |
| 05 | 23 | operand = 0x23 |
| 06 | 68 | SUB opcode |
| 07 | 22 | operand = 0x22 |
| 08 | 48 | STA opcode |
| 09 | 21 | operand = 0x21 |
| 0A | 98 | BST opcode |
| 0B | 00 | operand = 0x00 |

### Data (example values)

Example test case:
- x = 0x04
- z = 0x01
- 5 = 0x05

| Addr | Byte | Meaning |
|------|------|---------|
| 20 | 04 | x |
| 21 | 00 | y (initial) |
| 22 | 01 | z |
| 23 | 05 | constant 5 |

Expected result:
- y = 2*4 + 5 - 1 = 12 = 0x0C

---

## Expected Trace (Instruction Boundary)

This trace is evaluated after each instruction completes (i.e., after its execution phase,
after flags update).

Assume initial state:
- A = 0x00
- PAR = 0x00

### After `LDA 20`
- A = 0x04

### After `ADD 20`
- A = 0x08

### After `ADD 23`
- A = 0x0D

### After `SUB 22`
- A = 0x0C

### After `STA 21`
- Memory[0x21] = 0x0C
- A unchanged (still 0x0C)

### After `BST 00`
- Machine halted
- PAR updated per BST semantics (implementation must match docs)

---

## Expected Final Memory

- Memory[0x21] == 0x0C

---

## Optional Extended Trace (Bus-Oriented)

Recommended additional assertions:
- Each memory fetch passes through Z → Buffer
- STA passes A → Z → Buffer → Y → Memory

These are best validated with the debug pane’s trace log.

---

**End of Golden Program**
