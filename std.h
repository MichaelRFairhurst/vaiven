#ifndef VAIVEN_VISITOR_HEADER_STD
#define VAIVEN_VISITOR_HEADER_STD

#include "functions.h"
#include "heap.h"

namespace vaiven {

void init_std(Functions& funcs);

Value print(Value value);
Value newList();
Value newListWithSize(int size);
Value* getListContainerUnchecked(GcableList* list);
Value add(Value lhs, Value rhs);
Value addStrUnchecked(GcableString* lhs, GcableString* rhs);
Value append(Value lhs, Value rhs);
Value len(Value subject);
Value assert(Value expectation);
Value object();
Value keys(Value object);
Value set(Value objectOrArr, Value propOrIndex, Value value);
Value get(Value objectOrArr, Value propOrIndex);
Value listAccessUnchecked(GcableList* list, int index);
void listStoreUnchecked(GcableList* list, int index, Value value);
Value cmp(Value a, Value b);
Value inverseCmp(Value a, Value b);
// seems like using bool often results in garbage in the upper bits
uint64_t cmpUnboxed(Value a, Value b);
uint64_t cmpStrUnchecked(GcableString* a, GcableString* b);
uint64_t inverseCmpUnboxed(Value a, Value b);
uint64_t inverseCmpStrUnchecked(GcableString* a, GcableString* b);
Value toString(Value subject);
string toStringCpp(Value subject);

}

#endif
