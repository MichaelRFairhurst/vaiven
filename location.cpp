#include "location.h"

using namespace vaiven;
using namespace vaiven;

Location Location::imm(int val) {
  LocationDataUnion data;
  data.imm = val;
  return Location(LOCATION_TYPE_IMM, data);
}

Location Location::arg(int val) {
  LocationDataUnion data;
  data.argIndex = val;
  return Location(LOCATION_TYPE_ARG, data);
}

const asmjit::X86Gp* Location::getReg() {
  if (type == LOCATION_TYPE_REG) {
    return data.reg;
  } else if (type == LOCATION_TYPE_ARG) {
    switch (data.argIndex) {
      case 0: return &asmjit::x86::rdi;
      case 1: return &asmjit::x86::rsi;
      case 2: return &asmjit::x86::rdx;
      case 3: return &asmjit::x86::rcx;
      case 4: return &asmjit::x86::r8;
      case 5: return &asmjit::x86::r9;
    }
  }

  return NULL;
}

asmjit::X86Mem Location::getArgPtr() {
  // stack: ...|arg|arg|retaddr|rbp
  int stackQOffset = data.argIndex - 4;
  return asmjit::x86::ptr(asmjit::x86::rbp, stackQOffset * 8);
}
