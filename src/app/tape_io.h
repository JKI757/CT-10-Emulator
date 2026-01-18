#pragma once

#include <string>

#include "core/machine_state.h"

namespace ct10::app {

bool LoadTapeText(const std::string& path,
                  core::IOState& io,
                  std::string* error);
bool SaveTapeText(const std::string& path,
                  const core::IOState& io,
                  std::string* error);
bool SaveByteStream(const std::string& path,
                    const std::vector<uint8_t>& data,
                    std::string* error,
                    bool append_newline);
bool SaveAsciiText(const std::string& path,
                   const std::vector<uint8_t>& data,
                   std::string* error,
                   bool append_newline);
bool SaveHexDump(const std::string& path,
                 const std::vector<uint8_t>& data,
                 std::string* error);

}  // namespace ct10::app
