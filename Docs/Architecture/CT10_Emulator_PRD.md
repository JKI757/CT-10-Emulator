# COM-TRAN TEN (CT-10) High-Fidelity Emulator

## Product Requirements Document (PRD)

**Project Name:** CT-10 Emulator  
**Target Machine:** Digiac COM-TRAN TEN Training Computer  
**Primary Platform:** macOS  
**Language / Build:** C++20 + CMake  
**UI Framework:** Dear ImGui  
**Document Version:** 1.0

---

## 1. Purpose

Build a *museum-grade*, *instructionally accurate* emulator of the Digiac COM-TRAN TEN (CT-10) training computer.

The emulator must behave as if the original hardware exists behind the screen. Any switch press, key entry, or mode change must immediately propagate through all registers, internal buses, lamps, and displays exactly as defined by the original documentation.

This is not a performance emulator. It is a **behavioral digital twin**.

---

## 2. Core Fidelity Requirements

### 2.1 Timing Model

Timing must be logically accurate and user-adjustable.

* Adjustable execution speed: **1× to 10×**
* Two-layer timing model:
  * **Distributor pulses** (D-register 0–15)
  * **Clock pulses** (CP1, CP2, CP3) within each distributor pulse
* Instruction execution must be divided into:
  * Acquisition phase
  * Execution phase
  * Minor steps per clock pulse

Timing correctness is defined by **ordering and causality**, not wall-clock duration.

---

### 2.2 Registers (First-Class State)

All registers must exist as explicit stateful objects.

Required registers:

* Accumulator (A) – 8-bit
* Buffer (B) – 8-bit
* Quotient (Q) – 8-bit
* Index (X) – 8-bit
* Countdown (C) – 8-bit
* Memory Address Register (MAR) – 10-bit
* Program Address Register (PAR) – 10-bit
* OP Code Register
* Distributor Register (D) – 4-bit

Condition flags:

* Carry
* Zero
* Greater Than
* Less Than
* Add Overflow
* Divide Overflow

---

### 2.3 Internal Buses

Internal buses must be modeled as **first-class objects**.

Minimum required buses:

* X Bus
* Y Bus
* Z Bus
* F Bus

All register transfers must explicitly traverse buses.

---

## 3. Memory Model

* 1024 memory cells
* 8 bits per cell
* Instruction = two consecutive cells
* Memory access must pass through MAR and Buffer

Manual memory read/write workflows must block exactly as hardware.

---

## 4. Instruction Set

* All 44 documented instructions
* Correct immediate, indexed, and paged addressing
* Accurate multiply/divide AQ behavior
* Exact overflow and condition flag semantics

---

## 5. Control Panel & UI

### 5.1 Visual Accuracy

* Hex keypad
* Input switches
* Register lamps
* Mode switches
* START / CLEAR / LAMP TEST
* ERROR BYPASS switches

Lamp behavior is direct bit reflection. No smoothing.

---

### 5.2 Interaction Semantics

* Each UI interaction = hardware event
* START advances execution based on selected mode
* No abstraction of timing or batching

---

## 6. Debug & Introspection

Dedicated debug pane showing:

* All registers
* All internal buses
* Distributor count
* Clock phase (CP1/CP2/CP3)
* Acquisition vs execution phase
* Condition flags

---

## 7. Persistence & Media

### 7.1 Core Dumps

* Save/restore full machine state
* Resume execution exactly

### 7.2 Paper Tape

* Loadable after startup
* Must follow documented I/O behavior

---

## 8. Architecture

```
/core
  MachineState
  Register
  Bus
  Memory
  InstructionDecoder
  ExecutionEngine
  TimingEngine

/ui
  PanelLayout
  LampRenderer
  SwitchInput
  DisplayDecoder
  DebugPane

/app
  MainLoop
  ModeController
```

UI reflects state. UI never computes state.

---

## 9. Determinism & Testing

* Step execution at distributor or clock granularity
* Snapshot/restore at any point
* Golden reference tests

---

## 10. Non-Goals

* No performance shortcuts
* No abstraction of hardware behavior
* No semantic reinterpretation

---

## 11. Success Criteria

* Immediate lamp updates per key press
* Manual lab procedures work unmodified
* Micro-operation-level traceability
* Suitable for instruction and archival use

---

**End of PRD**
