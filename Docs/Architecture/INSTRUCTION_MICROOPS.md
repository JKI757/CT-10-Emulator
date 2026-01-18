# CT-10 Emulator – Instruction Micro-Operations

## Purpose

Defines instruction behavior at the micro-operation level.
Used to drive the Execution Engine.

---

## Example: LDA m,x

### Acquisition
1. CP1: PAR → MAR
2. CP2: Memory[MAR] → Buffer
3. CP3: Buffer → OP Code Register

### Execution
4. CP1: MAR + Index → Effective Address
5. CP2: Memory[EA] → Buffer
6. CP3: Buffer → Accumulator
7. CP3: Update condition flags

---

## Example: ADD m,x

1. Fetch operand into Buffer
2. CP1: A → ALU
3. CP2: Buffer → ALU
4. CP3: ALU → Accumulator
5. Update Carry and Overflow flags

---

## Example: DIV m,x

1. AQ treated as 16-bit dividend
2. CP2: Operand → ALU
3. CP3: Quotient → Q
4. CP3: Remainder → A
5. Update Divide Overflow flag

---

## Notes

- All register transfers must traverse buses
- Flags updated only at documented points
- No instruction completes outside Execution phase

---

**End of Micro-Operations**
