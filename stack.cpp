#include "stack.h"

#include "inttypes.h"

using namespace vaiven;

vaiven::StackFrame::StackFrame(uint64_t* prevRbp) {
  rbp = (uint64_t*) *prevRbp;
  retaddr = *(rbp + 8);
  locals = rbp - 8;
  uint64_t* lastLocal = prevRbp + 16;
  size = locals - lastLocal;
  locals = lastLocal; // so we can use locals[0], locals[1], ...
}

StackFrame vaiven::StackFrame::next() {
  return StackFrame(rbp);
}

bool vaiven::StackFrame::hasNext() {
  return *rbp != 0;
}

uint64_t* __attribute__ ((noinline)) vaiven::Stack::rbp() {
  asm("mov (%rbp), %rax");
}

StackFrame vaiven::Stack::top() {
  return StackFrame(rbp());
}
