# CT-10 Emulator – Architecture

## Overview

The emulator is structured as a deterministic hardware simulation, not a logical CPU model.
Every state transition corresponds to a documented hardware event.

Core principles:
- Explicit state
- No hidden side effects
- UI is a pure observer

---

## High-Level Layering

```
Application
 ├─ UI Layer (Dear ImGui)
 │   ├─ Control Panel
 │   ├─ Register Lamps
 │   ├─ Debug Pane
 │
 └─ Core Simulation
     ├─ MachineState
     ├─ Timing Engine
     ├─ Execution Engine
     ├─ Instruction Decoder
     ├─ Memory
     └─ Registers & Buses
```

---

## Core Objects

### MachineState
Single authoritative container.

Contains:
- All registers
- All buses
- Memory
- Timing state
- Mode state

### Register
Strongly typed value object.
- Bit width enforced
- Explicit load/clear/shift operations

### Bus
First-class signal carrier.
- Carries a value per clock pulse
- Cleared or overwritten explicitly

### Memory
- 1024 × 8-bit cells
- Access only via MAR + Buffer

---

## Timing Engine

Responsible for:
- Distributor pulses (0–15)
- Clock phases (CP1/CP2/CP3)
- Speed scaling (1×–10×)

No instruction logic exists here.

---

## Execution Engine

Consumes:
- Current instruction
- Timing events
- Bus availability

Produces:
- Register transfers
- Flag updates
- Control flow changes

---

## UI Contract

UI:
- Reads MachineState
- Emits user input events

UI never mutates registers directly.

---

**End of Architecture**
