#ifndef VAIVEN_VISITOR_HEADER_STACK
#define VAIVEN_VISITOR_HEADER_STACK

#include "inttypes.h"

namespace vaiven {

class StackFrame {
  public:
  StackFrame(uint64_t* prevRbp);

  int size;
  uint64_t* rbp;
  uint64_t* locals;
  uint64_t retaddr;

  StackFrame next();
  bool hasNext();
};

class Stack {
  public:
  StackFrame top();

  private:
  uint64_t* rbp();
};

}

#endif
