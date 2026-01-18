#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cmake -S "$root_dir" -B "$root_dir/build"
cmake --build "$root_dir/build"
