#include "app/tape_io.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "app/program_text.h"

namespace ct10::app {

bool LoadTapeText(const std::string& path,
                  core::IOState& io,
                  std::string* error) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file) {
    if (error) {
      *error = "Unable to open tape file.";
    }
    return false;
  }

  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();

  std::vector<uint8_t> bytes;
  ParseResult result;
  if (io.alpha_mode) {
    bytes.reserve(content.size());
    for (unsigned char c : content) {
      if (c == '\r') {
        continue;
      }
      bytes.push_back(static_cast<uint8_t>(c));
    }
    if (bytes.empty()) {
      if (error) {
        *error = "No bytes parsed from tape file.";
      }
      return false;
    }
  } else {
    result = ParseProgramText(content, bytes);
    if (bytes.empty()) {
      if (error) {
        *error = "No bytes parsed from tape file.";
      }
      return false;
    }
  }

  io.input_data = std::move(bytes);
  io.input_pos = 0;
  io.interrupt = false;

  if (!io.alpha_mode && error && result.skipped > 0) {
    *error = "Loaded with skipped tokens.";
  }
  return true;
}

bool SaveTapeText(const std::string& path,
                  const core::IOState& io,
                  std::string* error) {
  std::ofstream file(path);
  if (!file) {
    if (error) {
      *error = "Unable to open output file.";
    }
    return false;
  }

  if (io.hex_mode) {
    file << std::uppercase << std::hex << std::setfill('0');
    for (size_t i = 0; i < io.output_data.size(); ++i) {
      file << std::setw(2) << static_cast<unsigned>(io.output_data[i]) << ':';
      if ((i + 1) % 16 == 0) {
        file << "\n";
      }
    }
    if (!io.output_data.empty() && io.output_data.size() % 16 != 0) {
      file << "\n";
    }
  } else if (io.alpha_mode) {
    for (uint8_t value : io.output_data) {
      file << static_cast<char>(value);
    }
    if (!io.output_data.empty() &&
        io.output_data.back() != '\n') {
      file << '\n';
    }
  } else {
    file << std::uppercase << std::hex << std::setfill('0');
    for (size_t i = 0; i < io.output_data.size(); ++i) {
      if (i % 16 == 0 && i != 0) {
        file << '\n';
      }
      file << std::setw(2) << static_cast<unsigned>(io.output_data[i]);
      if (i % 16 != 15 && i + 1 < io.output_data.size()) {
        file << ' ';
      }
    }
    file << '\n';
  }

  if (!file) {
    if (error) {
      *error = "Failed to write output file.";
    }
    return false;
  }
  return true;
}

bool SaveByteStream(const std::string& path,
                    const std::vector<uint8_t>& data,
                    std::string* error,
                    bool append_newline) {
  std::ofstream file(path, std::ios::binary);
  if (!file) {
    if (error) {
      *error = "Unable to open output file.";
    }
    return false;
  }

  if (!data.empty()) {
    file.write(reinterpret_cast<const char*>(data.data()),
               static_cast<std::streamsize>(data.size()));
  }
  if (append_newline && (data.empty() || data.back() != '\n')) {
    file << '\n';
  }

  if (!file) {
    if (error) {
      *error = "Failed to write output file.";
    }
    return false;
  }
  return true;
}

bool SaveAsciiText(const std::string& path,
                   const std::vector<uint8_t>& data,
                   std::string* error,
                   bool append_newline) {
  std::ofstream file(path, std::ios::binary);
  if (!file) {
    if (error) {
      *error = "Unable to open output file.";
    }
    return false;
  }

  for (uint8_t value : data) {
    unsigned char c = static_cast<unsigned char>(value);
    if (c == '\r') {
      continue;
    }
    if (c == '\n' || c == '\t') {
      file << static_cast<char>(c);
    } else if (c >= 32 && c < 127) {
      file << static_cast<char>(c);
    } else {
      file << '.';
    }
  }

  if (append_newline && (data.empty() || data.back() != '\n')) {
    file << '\n';
  }

  if (!file) {
    if (error) {
      *error = "Failed to write output file.";
    }
    return false;
  }
  return true;
}

bool SaveHexDump(const std::string& path,
                 const std::vector<uint8_t>& data,
                 std::string* error) {
  std::ofstream file(path);
  if (!file) {
    if (error) {
      *error = "Unable to open output file.";
    }
    return false;
  }

  file << std::uppercase << std::hex << std::setfill('0');
  for (size_t i = 0; i < data.size(); ++i) {
    if (i % 16 == 0 && i != 0) {
      file << '\n';
    }
    file << std::setw(2) << static_cast<unsigned>(data[i]);
    if (i % 16 != 15 && i + 1 < data.size()) {
      file << ' ';
    }
  }
  file << '\n';

  if (!file) {
    if (error) {
      *error = "Failed to write output file.";
    }
    return false;
  }
  return true;
}

}  // namespace ct10::app
