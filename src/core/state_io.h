#pragma once

#include <string>

#include "core/machine_state.h"

namespace ct10::core {

bool SaveState(const MachineState& state,
               const std::string& path,
               std::string* error);
bool LoadState(MachineState& state,
               const std::string& path,
               std::string* error);

}  // namespace ct10::core
