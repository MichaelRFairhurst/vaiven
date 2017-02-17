#ifndef VAIVEN_HEADER_FUNCTION_USAGE
#define VAIVEN_HEADER_FUNCTION_USAGE

#include "inttypes.h"

#include "value.h"

namespace vaiven {

const short INT_SHAPE = INT_TAG >> 48;
const short BOOL_SHAPE = BOOL_TAG >> 48;
const short VOID_SHAPE = VOID >> 48;
const short DOUBLE_SHAPE = 0x08;
const short OBJECT_SHAPE = 0x10;

class ArgumentShape {
  public:
  short raw;

  bool isPure() {
    // is power of two trick
    return raw && !(raw & (raw - 1));
  }

  bool isPureInt() {
    return (raw & ~INT_SHAPE) == 0;
  }

  bool isPureBool() {
    return (raw & ~BOOL_SHAPE) == 0;
  }

  bool isPureVoid() {
    return (raw & ~VOID_SHAPE) == 0;
  }

  bool isPureDouble() {
    return (raw & ~DOUBLE_SHAPE) == 0;
  }

  bool isPureObject() {
    return (raw & ~OBJECT_SHAPE) == 0;
  }
  
};

class FunctionUsage {
  public:
  int32_t count;
  // variably sized -- allocate with new (alloca(...)) FunctionUsage
  ArgumentShape argShapes[0];
};

}

#endif
