# CT-10 Emulator – Full Signal Glossary → Enum Mapping

## Source

Extracted from the CT-10 training manual section titled **“SIGNAL NAME GLOSSARY”**.
This file is intended to be used directly by Codex to generate enums, string tables,
and debug-pane categories.

> Note: This is a mechanically extracted list. A small number of entries may need manual
> cleanup (e.g., duplicates or page-number anomalies) once you validate against the schematic set.

---

## Enum Proposal

```cpp
enum class Signal : uint16_t {
  AO_A7,
  ADD,
  ADEB,
  AEM,
  AND,
  APR,
  APHM,
  ARG,
  AXM,
  BO_B7,
  BNC,
  BNG,
  BPS,
  BST,
  BUN,
  BXZ,
  BZ,
  BZE,
  CCEQ,
  CCGT,
  CCLT,
  CKE,
  CL,
  CL,
  CLA,
  COL,
  COMP,
  COMP_A,
  COMP_Q,
  CP2,
  CP3,
  CR,
  CZ,
  DO_D3,
  DEC,
  DECA,
  DIV,
  DPO_DP15,
  DPAO_DPA15,
  DSLA,
  HF,
  HEX,
  HEXM,
  HX2,
  HXS,
  10_19,
  IBS,
  ICLK,
  INA,
  INCA,
  INCM,
  INEB,
  INM,
  INOC,
  INR,
  INR,
  INRD,
  INS,
  INSS,
  INT,
  INW,
  INWD,
  INX,
  IRB,
  IRD,
  ISO,
  IS2,
  IS3,
  IS4,
  IS5,
  IS6,
  IS7,
  IS8,
  IS9,
  ISB,
  IWB,
  IWT,
  LAI,
  LAMP_TEST,
  LAN,
  LAPH,
  LCC,
  LCG,
  LCI,
  LDA,
  LDQ,
  LF,
  LHEX,
  RST,
};
```

---

## Mapping Table

| Documentation Mnemonic | Enum ID | Reference | Notes |
|---|---|---|---|
| `AO -A7` | `AO_A7` | Sheet 1 | |
| `ADD` | `ADD` | Sheet 17 | |
| `ADEB` | `ADEB` | Sheet 15 | |
| `AEM` | `AEM` | Sheet 15 | |
| `AND` | `AND` | Sheet 17 | |
| `APR` | `APR` | Sheet 29 | |
| `APHM` | `APHM` | Sheet 29 | |
| `ARG` | `ARG` | Sheet 18 | |
| `AXM` | `AXM` | Sheet 23 | |
| `BO -B7` | `BO_B7` | Sheet 3 | |
| `BNC` | `BNC` | Sheet 17 | |
| `BNG` | `BNG` | Sheet 17 | |
| `BPS` | `BPS` | Sheet 17 | |
| `BST` | `BST` | Sheet 17 | |
| `BUN` | `BUN` | Sheet 17 | |
| `BXZ` | `BXZ` | Sheet 17 | |
| `BZ` | `BZ` | Sheet 3 | |
| `BZE` | `BZE` | Sheet 17 | |
| `CCEQ` | `CCEQ` | Sheet 19 | |
| `CCGT` | `CCGT` | Sheet 19 | |
| `CCLT` | `CCLT` | Sheet 19 | |
| `CKE` | `CKE` | Sheet 18 | |
| `CL` | `CL` | Sheet 15 | |
| `CL` | `CL` | Sheet 26 | |
| `CLA` | `CLA` | Sheet 23 | |
| `COL` | `COL` | Sheet 29 | |
| `COMP` | `COMP` | Sheet 20 | |
| `COMP A` | `COMP_A` | Sheet 20 | |
| `COMP Q` | `COMP_Q` | Sheet 20 | |
| `CP2` | `CP2` | Sheet 7 | |
| `CP3` | `CP3` | Sheet 7 | |
| `CR` | `CR` | Sheet 29 | |
| `CZ` | `CZ` | Sheet 4 | |
| `DO -D3` | `DO_D3` | Sheet 5 | |
| `DEC` | `DEC` | Sheet 24 | |
| `DECA` | `DECA` | Sheet 21 | |
| `DIV` | `DIV` | Sheet 17 | |
| `DPO -DP15` | `DPO_DP15` | Sheet 18 | |
| `DPAO -DPA15` | `DPAO_DPA15` | Sheet 18 | |
| `DSLA` | `DSLA` | Sheet 24 | |
| `HF` | `HF` | Sheet 15 | |
| `HEX` | `HEX` | Sheet 29 | |
| `HEXM` | `HEXM` | Sheet 29 | |
| `HX2` | `HX2` | Sheet 29 | |
| `HXS` | `HXS` | Sheet 25 | |
| `10 -19` | `10_19` | Sheet 10 | |
| `IBS` | `IBS` | Sheet 24 | |
| `ICLK` | `ICLK` | Sheet 15 | |
| `INA` | `INA` | Sheet 22 | |
| `INCA` | `INCA` | Sheet 21 | |
| `INCM` | `INCM` | Sheet 24 | |
| `INEB` | `INEB` | Sheet 15 | |
| `INM` | `INM` | Sheet 15 | |
| `INOC` | `INOC` | Sheet 20 | |
| `INR` | `INR` | Sheet 20 | |
| `INR` | `INR` | Sheet 29 | |
| `INRD` | `INRD` | Sheet 20 | |
| `INS` | `INS` | Sheet 22 | |
| `INSS` | `INSS` | Sheet 20 | |
| `INT` | `INT` | Sheet 19 | |
| `INW` | `INW` | Sheet 20 | |
| `INWD` | `INWD` | Sheet 29 | |
| `INX` | `INX` | Sheet 17 | |
| `IRB` | `IRB` | Sheet 15 | |
| `IRD` | `IRD` | Sheet 15 | |
| `ISO` | `ISO` | Sheet 25 | |
| `IS2` | `IS2` | Sheet 25 | |
| `IS3` | `IS3` | Sheet 25 | |
| `IS4` | `IS4` | Sheet 25 | |
| `IS5` | `IS5` | Sheet 25 | |
| `IS6` | `IS6` | Sheet 25 | |
| `IS7` | `IS7` | Sheet 25 | |
| `IS8` | `IS8` | Sheet 25 | |
| `IS9` | `IS9` | Sheet 25 | |
| `ISB` | `ISB` | Sheet 24 | |
| `IWB` | `IWB` | Sheet 15 | |
| `IWT` | `IWT` | Sheet 15 | |
| `LAI` | `LAI` | Sheet 17 | |
| `LAMP TEST` | `LAMP_TEST` | Sheet 26 | |
| `LAN` | `LAN` | Sheet 17 | |
| `LAPH` | `LAPH` | Sheet 10 | |
| `LCC` | `LCC` | Sheet 17 | |
| `LCG` | `LCG` | Sheet 18 | |
| `LCI` | `LCI` | Sheet 18 | |
| `LDA` | `LDA` | Sheet 17 | |
| `LDQ` | `LDQ` | Sheet 17 | |
| `LF` | `LF` | Sheet 29 | |
| `LHEX` | `LHEX` | Sheet 10 | |
| `RST` | `RST` | Sheet 11 | |

---

## Next Step (Recommended)

1. Add a `SignalName(Signal)` function using the table above.
2. Add a `SignalCategory(Signal)` classifier (Timing / Transfer / ALU / Error / Panel / I/O).
3. Expand this file if additional glossary pages exist outside the initial extraction window.

---

**End of Mapping**
