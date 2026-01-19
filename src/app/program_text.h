#pragma once

#include <array>
#include <cctype>
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

enum class AsmAddressing {
  Immediate,
  Paged,
};

struct AssemblerOp {
  std::string_view mnemonic;
  uint8_t opcode;
  AsmAddressing addressing;
};

inline constexpr std::array<AssemblerOp, 44> kAssemblerOps = {{
    {"SST", 0x00, AsmAddressing::Immediate},
    {"LCI", 0x01, AsmAddressing::Immediate},
    {"LAI", 0x02, AsmAddressing::Immediate},
    {"INX", 0x03, AsmAddressing::Immediate},
    {"SKI", 0x08, AsmAddressing::Immediate},
    {"SKS", 0x09, AsmAddressing::Immediate},
    {"SKF", 0x0A, AsmAddressing::Immediate},
    {"SLA", 0x0B, AsmAddressing::Immediate},
    {"SRA", 0x10, AsmAddressing::Immediate},
    {"OCD", 0x11, AsmAddressing::Immediate},
    {"LXI", 0x12, AsmAddressing::Immediate},
    {"SLL", 0x13, AsmAddressing::Immediate},
    {"SRL", 0x18, AsmAddressing::Immediate},
    {"AND", 0x19, AsmAddressing::Immediate},
    {"IOR", 0x1A, AsmAddressing::Immediate},
    {"XOR", 0x1B, AsmAddressing::Immediate},
    {"FLC", 0x28, AsmAddressing::Immediate},
    {"FLS", 0xF8, AsmAddressing::Immediate},
    {"LDA", 0x20, AsmAddressing::Paged},
    {"LCC", 0x30, AsmAddressing::Paged},
    {"LAN", 0x38, AsmAddressing::Paged},
    {"LDQ", 0x40, AsmAddressing::Paged},
    {"STA", 0x48, AsmAddressing::Paged},
    {"STX", 0x50, AsmAddressing::Paged},
    {"STQ", 0x58, AsmAddressing::Paged},
    {"ADD", 0x60, AsmAddressing::Paged},
    {"SUB", 0x68, AsmAddressing::Paged},
    {"MPY", 0x70, AsmAddressing::Paged},
    {"DIV", 0x78, AsmAddressing::Paged},
    {"RAO", 0x80, AsmAddressing::Paged},
    {"RSO", 0x88, AsmAddressing::Paged},
    {"BUN", 0x90, AsmAddressing::Paged},
    {"BST", 0x98, AsmAddressing::Paged},
    {"BSB", 0xA0, AsmAddressing::Paged},
    {"BPS", 0xA8, AsmAddressing::Paged},
    {"BZE", 0xB0, AsmAddressing::Paged},
    {"BNG", 0xB8, AsmAddressing::Paged},
    {"BNC", 0xC0, AsmAddressing::Paged},
    {"BXZ", 0xC8, AsmAddressing::Paged},
    {"WDB", 0xD0, AsmAddressing::Paged},
    {"MNO", 0xD8, AsmAddressing::Paged},
    {"RDB", 0xE0, AsmAddressing::Paged},
    {"RDI", 0xE8, AsmAddressing::Paged},
    {"MNI", 0xF0, AsmAddressing::Paged},
}};

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

inline char ToUpperChar(char c) {
  return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

inline std::string ToUpperToken(std::string_view token) {
  std::string upper;
  upper.reserve(token.size());
  for (char c : token) {
    upper.push_back(ToUpperChar(c));
  }
  return upper;
}

inline bool IsIndexToken(std::string_view token) {
  return token.size() == 1 &&
         (token[0] == 'X' || token[0] == 'x');
}

inline const AssemblerOp* FindAssemblerOp(std::string_view token) {
  std::string upper = ToUpperToken(token);
  for (const auto& op : kAssemblerOps) {
    if (upper == op.mnemonic) {
      return &op;
    }
  }
  return nullptr;
}

inline bool IsSeparator(char c) {
  return c == ' ' || c == '\t' || c == ',' || c == ';' || c == ':' || c == '.';
}

inline std::vector<std::string> TokenizeLine(std::string_view line) {
  std::vector<std::string> tokens;
  std::string token;
  for (char c : line) {
    if (IsSeparator(c)) {
      if (!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
      continue;
    }
    token.push_back(c);
  }
  if (!token.empty()) {
    tokens.push_back(token);
  }
  return tokens;
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

    std::vector<std::string> tokens = TokenizeLine(code);
    if (tokens.empty()) {
      continue;
    }

    const AssemblerOp* op = nullptr;
    int mnemonic_index = -1;
    for (int i = 0; i < static_cast<int>(tokens.size()); ++i) {
      op = FindAssemblerOp(tokens[i]);
      if (op) {
        mnemonic_index = i;
        break;
      }
    }

    if (op) {
      for (const auto& token : tokens) {
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
        }
      }

      int operand_index = mnemonic_index + 1;
      bool indexed = false;
      if (operand_index < static_cast<int>(tokens.size()) &&
          IsIndexToken(tokens[operand_index])) {
        indexed = true;
        ++operand_index;
      }

      if (operand_index >= static_cast<int>(tokens.size())) {
        ++result.skipped;
        continue;
      }

      uint16_t operand = 0;
      if (!ParseHexToken(tokens[operand_index], operand, false)) {
        ++result.skipped;
        continue;
      }

      if (!indexed && operand_index + 1 < static_cast<int>(tokens.size()) &&
          IsIndexToken(tokens[operand_index + 1])) {
        indexed = true;
      }

      uint8_t opcode = 0;
      uint8_t immediate = static_cast<uint8_t>(operand & 0xFF);

      if (op->addressing == AsmAddressing::Immediate) {
        if (indexed || operand > 0xFF) {
          ++result.skipped;
          continue;
        }
        opcode = op->opcode;
      } else {
        if (operand > 0x3FF) {
          ++result.skipped;
          continue;
        }
        uint8_t page = static_cast<uint8_t>((operand >> 8) & 0x03);
        opcode = static_cast<uint8_t>(op->opcode | page |
                                      (indexed ? 0x04u : 0x00u));
      }

      if (!cursor_set) {
        cursor_set = true;
      }
      spec.writes.push_back({cursor, opcode});
      cursor = static_cast<uint16_t>((cursor + 1) & 0x3FF);
      spec.writes.push_back({cursor, immediate});
      cursor = static_cast<uint16_t>((cursor + 1) & 0x3FF);
      result.parsed += 2;
      continue;
    }

    for (const auto& token : tokens) {
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
