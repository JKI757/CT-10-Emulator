# CT-10 (COM-TRAN TEN) Emulator

This repo builds a museum-grade emulator of the Digiac COM-TRAN TEN (CT-10) training
computer, with a full front panel, cycle-accurate timing, and a hardware-faithful core
simulation. The emulator is a behavioral digital twin rather than a high-level CPU model.

## Historical context (from Docs/)

- Navy training at Great Lakes: "COM-TRAN TEN Logic Diagrams for Combat Systems Advanced
  Digital A-100-0142 (Volume 4)" prepared by Service School Command, Great Lakes, IL
  (Sept 1995) for the Chief of Naval Education and Training.
- Navy Data Systems Technician training at Mare Island: "Student Guide for Data Systems
  Technician Class A Phase Two A-150-0025 Volume II (Part 3)" prepared by Combat Systems
  Technical Schools Command, Mare Island, California (01 Apr 1991).
- Earlier Air Force training use: "Electronic Computer and Switching Systems Specialist:
  Computer Units and COM-TRAN 10" (June 1981) from USAF Technical Training School,
  3380th Technical Training Group, Keesler Air Force Base, Mississippi.

Sources: `Docs/Historical/comtran10.pdf`,
`Docs/Historical/KDA-3032_Digiac_COM-TRAN_TEN_Training_Jun81 ct10 ct-10.pdf`,
`Docs/Historical/COM-TRAN10a.jpg`.

## Navy DS rating notes

- TODO: Fill in DS rating history with official Navy sources.

## Device capabilities (as documented)

- 8-bit word size with 8-bit A/B/Q/X/C registers; a word is two hex digits and represents
  values from +127 to -128.
- 1024 memory cells (0x000 to 0x3FF) arranged as 400 hex locations; upper memory is accessed
  by paging the opcode.
- 44 instructions across load, store, arithmetic, logical, branch, and I/O categories.
- Two-phase instruction cycle: acquisition and execution.
- Physical control panel with switches, hex keypad, register lamps, and manual memory entry.

Primary sources: `Docs/Historical/KDA-3032_Digiac_COM-TRAN_TEN_Training_Jun81 ct10 ct-10.pdf`,
`Docs/Architecture/CT10_Emulator_PRD.md`.

## Emulator goals and features

- Cycle-accurate timing with distributor pulses and CP1/CP2/CP3 clocks; 1x to 10x speed
  scaling.
- Explicit register, bus, and memory objects with deterministic machine state.
- UI modeled after the COM-TRAN TEN control panel, plus a debug pane for buses, flags, and
  timing phase.

Documentation: `Docs/Architecture/CT10_Emulator_PRD.md`,
`Docs/Architecture/ARCHITECTURE.md`, `Docs/Architecture/TIMING_MODEL.md`.

Note: The PRD and architectural documents were written by ChatGPT 5.2 using the sources in Docs/Historical, and the code was written by Codex using gpt-codex-5.2.

## Build and run

Requirements: CMake 3.20+, a C++20 compiler, OpenGL, and internet access for FetchContent
(GLFW and Dear ImGui).

```bash
./scripts/build.sh
```

Run the UI emulator:

```bash
./build/ct10
```

Run the headless core:

```bash
./build/ct10_headless
```

Headless options (optional program/tape input):

```bash
./build/ct10_headless tests/programs/test.txt --max-steps 200000
./build/ct10_headless tests/programs/test.txt --tape /path/to/input.txt --tape-hex
./build/ct10_headless --tape /path/to/terminal.txt --tape-alpha
./build/ct10_headless tests/programs/io_term_printer.txt \
  --terminal-in tests/tapes/terminal_input.txt --terminal-alpha \
  --expect-term tests/expected/terminal_output.hex \
  --expect-printer tests/expected/printer_output.hex
./build/ct10_headless tests/programs/io_term_printer.txt --io-mode alpha
```

I/O demo (terminal input echoed to terminal + printer):

```bash
./build/ct10_headless tests/programs/io_term_printer.txt --terminal-in tests/tapes/terminal_input.txt --terminal-alpha
```

UI tip: Terminal/Printer outputs can be saved from the Program pane in Raw/Text/Hex
formats with optional timestamps.

I/O regression:

```bash
./scripts/test_io.sh
```

## Fonts

The UI loads fonts from `assets/fonts`, but the font files are gitignored. Copy
OTF/TTF files into `assets/fonts` to enable the listed options in
`src/ui/panel_layout.h`.

Suggested sources (and licenses):
- TeX Gyre family (GUST Font License): https://www.gust.org.pl/projects/e-foundry/tex-gyre/
- Cousine (SIL OFL): https://fonts.google.com/specimen/Cousine
- IBM Plex Mono (SIL OFL): https://fonts.google.com/specimen/IBM+Plex+Mono
- Fira Mono (SIL OFL): https://fonts.google.com/specimen/Fira+Mono

## Docs to read next

- Product requirements and fidelity constraints: `Docs/Architecture/CT10_Emulator_PRD.md`
- Core architecture: `Docs/Architecture/ARCHITECTURE.md`
- Timing and micro-ops: `Docs/Architecture/TIMING_MODEL.md`,
  `Docs/Architecture/INSTRUCTION_MICROOPS.md`
- Golden lab program traces: `Docs/Architecture/GOLDEN_LAB_PROGRAM_TRACES.md`

## Images

Historical device and training materials:

![COM-TRAN TEN front elevation](Docs/Historical/FrontElevation.jpg)
![COM-TRAN TEN educational system](Docs/Historical/COM-TRAN10c.jpg)

Emulator UI:

![CT-10 emulator screenshot](Docs/Output/Screenshot%202026-01-19%20at%2012.03.35.png)

## Dependencies and licenses

- Dear ImGui: MIT license.
- GLFW: zlib/libpng license.
- OpenGL: platform-provided Khronos API (system library).
- CMake: BSD-3-Clause (build tool).
