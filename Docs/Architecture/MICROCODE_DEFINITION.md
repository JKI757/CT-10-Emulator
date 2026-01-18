# CT-10 Emulator – Microcode Definition (Codex-Friendly)

## Purpose

Defines instruction behavior as declarative micro-operations.
This format is intended to be translated directly into C++ tables.

---

## Conceptual Structure

```yaml
instruction:
  name: LDA
  addressing: indexed
  microops:
    - dist: 0
      cp: 1
      action: PAR_TO_MAR
    - dist: 0
      cp: 2
      action: MEM_TO_Z
    - dist: 0
      cp: 3
      action: Z_TO_BUFFER
    - dist: 1
      cp: 1
      action: BUFFER_TO_OPCODE
```

---

## Example: LDA m,x

```yaml
name: LDA
opcode: 0x20
microops:
  - { dist: 2, cp: 1, op: ADD_INDEX_TO_MAR }
  - { dist: 2, cp: 2, op: MEM_TO_Z }
  - { dist: 2, cp: 3, op: Z_TO_BUFFER }
  - { dist: 3, cp: 1, op: BUFFER_TO_F }
  - { dist: 3, cp: 2, op: F_TO_ACCUMULATOR }
  - { dist: 3, cp: 3, op: UPDATE_FLAGS }
```

---

## Supported Primitive Ops

- PAR_TO_MAR
- MEM_TO_Z
- Z_TO_BUFFER
- BUFFER_TO_OPCODE
- ADD_INDEX_TO_MAR
- BUFFER_TO_F
- F_TO_ACCUMULATOR
- ALU_ADD
- ALU_SUB
- ALU_DIV
- ALU_MUL
- UPDATE_FLAGS
- UPDATE_OVERFLOW

---

## Rules

- One operation per CP
- All transfers must use a bus
- Flags updated only at explicit micro-ops

---

**End of Microcode Definition**
