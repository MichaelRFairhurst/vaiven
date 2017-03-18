#ifndef VAIVEN_VISITOR_HEADER_TYPE_INFO
#define VAIVEN_VISITOR_HEADER_TYPE_INFO

#include "asmjit/src/asmjit/asmjit.h"

#include "location.h"

namespace vaiven {

enum VaivenStaticType {
  VAIVEN_STATIC_TYPE_VOID,
  VAIVEN_STATIC_TYPE_INT,
  VAIVEN_STATIC_TYPE_BOOL,
  VAIVEN_STATIC_TYPE_DOUBLE,
  VAIVEN_STATIC_TYPE_STRING,
  VAIVEN_STATIC_TYPE_OBJECT,
  VAIVEN_STATIC_TYPE_LIST,
  VAIVEN_STATIC_TYPE_UNKNOWN,
};

class TypedLocationInfo {

  public:
  TypedLocationInfo(Location location, VaivenStaticType type, bool isBoxed)
    : location(location), type(type), isBoxed(isBoxed) {};

  TypedLocationInfo() : location(), type(VAIVEN_STATIC_TYPE_UNKNOWN) {};

  VaivenStaticType type;
  Location location;
  bool isBoxed;

};

}

#endif
