#include "value.h"
#include "heap.h"

using namespace vaiven;

VaivenStaticType Value::getStaticType() const {
  return isInt()
      ? VAIVEN_STATIC_TYPE_INT
      : isBool()
      ? VAIVEN_STATIC_TYPE_BOOL
      : isVoid()
      ? VAIVEN_STATIC_TYPE_VOID
      : isDouble()
      ? VAIVEN_STATIC_TYPE_DOUBLE
      : getPtr()->getType() == GCABLE_TYPE_STRING
      ? VAIVEN_STATIC_TYPE_STRING
      : getPtr()->getType() == GCABLE_TYPE_LIST
      ? VAIVEN_STATIC_TYPE_LIST
      : VAIVEN_STATIC_TYPE_OBJECT;
}
