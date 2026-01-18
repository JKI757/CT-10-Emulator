#pragma once

#include <cstdlib>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace ct10::app {

struct ParseResult {
  int parsed = 0;
  int skipped = 0;
};

struct ProgramWrite {
  uint16_t address = 0;
  uint8_t value = 0;
};

struct Expectation {
  uint16_t address = 0;
  uint8_t value = 0;
};

struct ProgramSpec {
  uint16_t entry = 0;
  bool has_entry = false;
  bool uses_addresses = false;
  std::vector<ProgramWrite> writes;
  std::vector<Expectation> expects;
};

inline bool ParseHexToken(const std::string& token,
                          uint16_t& value,
                          bool byte_only) {
  std::string cleaned = token;
  if (cleaned.size() >= 2 &&
      (cleaned.rfind("0x", 0) == 0 || cleaned.rfind("0X", 0) == 0)) {
    cleaned = cleaned.substr(2);
  }
  if (cleaned.empty()) {
    return false;
  }
  char* end = nullptr;
  unsigned long parsed = std::strtoul(cleaned.c_str(), &end, 16);
  if (end == cleaned.c_str() || *end != '\0') {
    return false;
  }
  if (byte_only && parsed > 0xFF) {
    return false;
  }
  if (!byte_only && parsed > 0xFFFF) {
    return false;
  }
  value = static_cast<uint16_t>(parsed);
  return true;
}

inline std::string SanitizeProgramText(std::string_view text) {
  std::string cleaned;
  cleaned.reserve(text.size());
  bool in_comment = false;
  for (char c : text) {
    if (in_comment) {
      if (c == '\n') {
        in_comment = false;
      }
      continue;
    }
    if (c == '#') {
      in_comment = true;
      continue;
    }
    if (c == ',' || c == ';' || c == ':') {
      cleaned.push_back(' ');
      continue;
    }
    cleaned.push_back(c);
  }
  return cleaned;
}

inline ParseResult ParseProgramText(std::string_view text,
                                    std::vector<uint8_t>& bytes) {
  ParseResult result;
  bytes.clear();
  if (text.empty()) {
    return result;
  }

  std::string cleaned = SanitizeProgramText(text);
  std::istringstream stream(cleaned);
  std::string token;
  while (stream >> token) {
    if (token.size() >= 2 &&
        (token.rfind("0x", 0) == 0 || token.rfind("0X", 0) == 0)) {
      token = token.substr(2);
    }
    if (token.empty()) {
      continue;
    }

    char* end = nullptr;
    unsigned long value = std::strtoul(token.c_str(), &end, 16);
    if (end == token.c_str() || *end != '\0' || value > 0xFF) {
      ++result.skipped;
      continue;
    }
    bytes.push_back(static_cast<uint8_t>(value));
  }

  result.parsed = static_cast<int>(bytes.size());
  return result;
}

inline void ParseDirective(const std::string& directive, ProgramSpec& spec) {
  std::istringstream stream(directive);
  std::string keyword;
  if (!(stream >> keyword)) {
    return;
  }
  if (keyword == "START") {
    std::string addr_token;
    if (stream >> addr_token) {
      uint16_t addr = 0;
      if (ParseHexToken(addr_token, addr, false)) {
        spec.entry = static_cast<uint16_t>(addr & 0x3FF);
        spec.has_entry = true;
      }
    }
    return;
  }
  if (keyword == "EXPECT") {
    std::string addr_token;
    std::string value_token;
    if ((stream >> addr_token) && (stream >> value_token)) {
      uint16_t addr = 0;
      uint16_t value = 0;
      if (ParseHexToken(addr_token, addr, false) &&
          ParseHexToken(value_token, value, true)) {
        spec.expects.push_back(
            {static_cast<uint16_t>(addr & 0x3FF),
             static_cast<uint8_t>(value & 0xFF)});
      }
    }
  }
}

inline void ParseProgramContent(std::string_view text,
                                ProgramSpec& spec,
                                ParseResult& result) {
  spec = {};
  result = {};

  std::string text_copy(text);
  std::istringstream lines(text_copy);
  std::string line;
  uint16_t cursor = 0;
  bool cursor_set = false;

  while (std::getline(lines, line)) {
    std::string code = line;
    std::string comment;
    size_t hash = line.find('#');
    if (hash != std::string::npos) {
      code = line.substr(0, hash);
      comment = line.substr(hash + 1);
      while (!comment.empty() &&
             (comment.front() == ' ' || comment.front() == '\t')) {
        comment.erase(comment.begin());
      }
      if (!comment.empty()) {
        ParseDirective(comment, spec);
      }
    }

    for (char& c : code) {
      if (c == ',' || c == ';' || c == ':') {
        c = ' ';
      }
    }

    std::istringstream tokens(code);
    std::string token;
    while (tokens >> token) {
      if (!token.empty() && token.front() == '@') {
        uint16_t addr = 0;
        if (ParseHexToken(token.substr(1), addr, false)) {
          cursor = static_cast<uint16_t>(addr & 0x3FF);
          cursor_set = true;
          spec.uses_addresses = true;
          if (!spec.has_entry && spec.writes.empty()) {
            spec.entry = cursor;
            spec.has_entry = true;
          }
        } else {
          ++result.skipped;
        }
        continue;
      }

      uint16_t value = 0;
      if (ParseHexToken(token, value, true)) {
        if (!cursor_set) {
          cursor_set = true;
        }
        spec.writes.push_back(
            {cursor, static_cast<uint8_t>(value & 0xFF)});
        cursor = static_cast<uint16_t>((cursor + 1) & 0x3FF);
        ++result.parsed;
      } else {
        ++result.skipped;
      }
    }
  }
}

}  // namespace ct10::app
