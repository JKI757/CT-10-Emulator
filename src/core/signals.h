#pragma once

#include <cstdint>
#include <string_view>

namespace ct10::core {

enum class Signal : uint16_t {
  CP1,
  CP2,
  CP3,
  DPA0,
  DPA1,
  DPA2,
  DPA3,
  DPA4,
  DPA5,
  DPA6,
  DPA7,
  DPA8,
  DPA9,
  DPA10,
  DPA11,
  DPA12,
  DPA13,
  DPA14,
  DPA15,
  DP0,
  DP1,
  DP2,
  DP3,
  DP4,
  DP5,
  DP6,
  DP7,
  DP8,
  DP9,
  DP10,
  DP11,
  DP12,
  DP13,
  DP14,
  DP15,
  TAZ,
  TZB,
  TBA,
  TAB,
  TQB,
  TBQ,
  TXB,
  TBX,
  TPM,
  TMP,
  TBM,
  TGM,
  TYC,
  TIY,
  TIM,
  TIP,
  TIS,
  ISB,
  IBS,
  INA,
  INS,
  INCA,
  DECA,
  IAND,
  IORI,
  IEX,
  STCC,
  CARY,
  ADD_OVFL,
  DIV_OVFL,
  START,
  STOP,
  CLEAR,
  LAMP_TEST,
  DIST_MODE,
  AE_MODE,
  INST_MODE,
  PROG_MODE,
};

constexpr std::string_view SignalName(Signal signal) {
  switch (signal) {
    case Signal::CP1:
      return "CP1";
    case Signal::CP2:
      return "CP2";
    case Signal::CP3:
      return "CP3";
    case Signal::TAZ:
      return "TAZ";
    case Signal::TZB:
      return "TZB";
    case Signal::TBA:
      return "TBA";
    case Signal::TAB:
      return "TAB";
    case Signal::ISB:
      return "ISB";
    case Signal::IBS:
      return "IBS";
    case Signal::START:
      return "START";
    case Signal::STOP:
      return "STOP";
    case Signal::CLEAR:
      return "CLEAR";
    case Signal::LAMP_TEST:
      return "LAMP_TEST";
    default:
      return "UNKNOWN";
  }
}

}  // namespace ct10::core
