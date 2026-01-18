#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)

"$root/build/ct10_headless" \
  "$root/tests/programs/io_term_printer.txt" \
  --terminal-in "$root/tests/tapes/terminal_input.txt" --terminal-alpha \
  --expect-term "$root/tests/expected/terminal_output.hex" \
  --expect-printer "$root/tests/expected/printer_output.hex"
