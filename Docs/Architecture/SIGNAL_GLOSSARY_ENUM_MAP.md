# CT-10 Emulator – Signal Glossary → Enum Mapping (Starter Set)

## Purpose

Define a stable mapping between CT-10 “signal mnemonics” and implementation enums/IDs.
This enables:
- Trace logging with meaningful names
- Table-driven microcode execution
- Debug pane signal visualization

This is a **starter set** intended to be expanded as you ingest the schematic signal glossary.

---

## Enum Strategy

Use strongly typed enums.

Suggested structure:

```cpp
enum class Signal : uint16_t {
  // Timing
  CP1, CP2, CP3,
  DPA0, DPA1, /*...*/ DPA15,
  DP0, DP1, /*...*/ DP15,

  // Transfers (register/bus)
  TAZ, TZB, TBA, TAB,
  TQB, TBQ,
  TXB, TBX,
  TPM, TMP,
  TBM, TGM,
  TYC,
  TIY, TIM, TIP, TIS,

  // Memory control
  ISB, IBS,

  // ALU control
  INA, INS, INCA, DECA,
  IAND, IORI, IEX,

  // Flags / errors
  STCC,
  CARY,
  ADD_OVFL,
  DIV_OVFL,

  // Control panel / modes
  START, STOP, CLEAR, LAMP_TEST,
  DIST_MODE, AE_MODE, INST_MODE, PROG_MODE,
};
```

---

## Suggested Name Normalization

Rules:
- Use the mnemonic from documentation where possible (e.g., TAZ)
- Prefer uppercase with underscores only where needed for C++ identifiers
- If the docs distinguish complemented forms, model them as separate signals only if required

---

## Starter Mapping Table

| Documentation Mnemonic | Enum | Notes |
|---|---|---|
| CP1 | CP1 | Clock pulse 1 |
| CP2 | CP2 | Clock pulse 2 |
| CP3 | CP3 | Clock pulse 3 |
| DPA0..DPA15 | DPA0..DPA15 | Acquisition distributor pulses |
| DP0..DP15 | DP0..DP15 | Execution distributor pulses |
| TAZ | TAZ | Transfer A → Z bus |
| TZB | TZB | Transfer Z → Buffer |
| TBA | TBA | Transfer Buffer → A (via Y/F) |
| TAB | TAB | Transfer A → Buffer |
| ISB | ISB | Initiate Storage → Buffer (memory read) |
| IBS | IBS | Initiate Buffer → Storage (memory write) |
| AXM | (micro-op) | Add Index → MAR (often better as a micro-op than a raw signal) |
| TGM | TGM | G bus → MAR latch |
| TBM | TBM | Buffer → MAR (operand) |
| TPM | TPM | Program address → MAR |
| TMP | TMP | MAR → Program address |
| LX | (micro-op) | Load Index (often better as a micro-op) |
| TYC | TYC | Y bus → Countdown latch |
| STCC | STCC | Set condition code (typically DP15) |

---

## Expansion Plan (Recommended)

1. Extract full signal glossary from the CT-10 docs
2. Add each mnemonic to `Signal` enum
3. Create `std::string_view SignalName(Signal)`
4. Add `SignalCategory` for UI filtering

---

**End of Enum Mapping**
