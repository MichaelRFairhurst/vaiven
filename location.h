#ifndef VAIVEN_VISITOR_HEADER_LOCATION
#define VAIVEN_VISITOR_HEADER_LOCATION

#include "asmjit/src/asmjit/asmjit.h"

namespace vaiven {

enum LocationType {
  LOCATION_TYPE_REG,
  LOCATION_TYPE_IMM,
  LOCATION_TYPE_SPILLED,
};

typedef union {
    asmjit::X86Gp* reg;
    int imm;
} LocationDataUnion;

class Location {

  public:
  Location(asmjit::X86Gp* reg) : data(), type(LOCATION_TYPE_REG) {
    data.reg = reg;
  }
  Location(int imm) : data(), type(LOCATION_TYPE_IMM) {
    data.imm = imm;
  }
  Location() : data(), type(LOCATION_TYPE_SPILLED) {}

  LocationType type;
  LocationDataUnion data;

};

}

#endif
