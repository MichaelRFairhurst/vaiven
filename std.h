#ifndef VAIVEN_VISITOR_HEADER_STD
#define VAIVEN_VISITOR_HEADER_STD

#include "functions.h"

namespace vaiven {

void init_std(Functions& funcs);

Value print(Value value);
Value listWith(Value value);

}

#endif
