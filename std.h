#ifndef VAIVEN_VISITOR_HEADER_STD
#define VAIVEN_VISITOR_HEADER_STD

#include "functions.h"
#include "heap.h"

namespace vaiven {

void init_std(Functions& funcs);

Value print(Value value);
Value newList();
Value add(Value lhs, Value rhs);
Value addStrUnchecked(GcableString* lhs, GcableString* rhs);
Value append(Value lhs, Value rhs);
Value len(Value subject);
Value assert(Value expectation);
Value object();
Value keys(Value object);
Value set(Value objectOrArr, Value propOrIndex, Value value);
Value get(Value objectOrArr, Value propOrIndex);
Value cmp(Value a, Value b);
Value inverseCmp(Value a, Value b);
bool cmpUnboxed(Value a, Value b);
bool cmpStrUnchecked(GcableString* a, GcableString* b);
bool inverseCmpUnboxed(Value a, Value b);
bool inverseCmpStrUnchecked(GcableString* a, GcableString* b);
Value toString(Value subject);
string toStringCpp(Value subject);

}

#endif
