#ifndef VAIVEN_VISITOR_HEADER_STD
#define VAIVEN_VISITOR_HEADER_STD

#include "functions.h"

namespace vaiven {

void init_std(Functions& funcs);

Value print(Value value);
Value newList();
Value append(Value lhs, Value rhs);
Value len(Value subject);
Value assert(Value expectation);
Value object();
Value keys(Value object);
Value set(Value objectOrArr, Value propOrIndex, Value value);
Value get(Value objectOrArr, Value propOrIndex);
Value cmp(Value a, Value b);
Value toString(Value subject);

}

#endif
