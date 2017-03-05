#include "forward_visitor.h"
#include "cfg.h"

using namespace vaiven::ssa;
using std::unique_ptr;

void ForwardVisitor::visitBlock(Block& block) {
  curBlock = &block;
  lastInstr = NULL;
  Instruction* next = block.head.get();
  while (next != NULL) {
    next->accept(*this);
    lastInstr = next;
    next = next->next;
  }

  for (vector<unique_ptr<BlockExit>>::iterator it = block.exits.begin();
      it != block.exits.end();
      ++it) {
    (*it)->accept(*this);
  }

  if (block.next != NULL) {
    block.next->accept(*this);
  }
}

void ForwardVisitor::visitUnconditionalBlockExit(UnconditionalBlockExit& exit) {
  // noop
}

void ForwardVisitor::visitConditionalBlockExit(ConditionalBlockExit& exit) {
  exit.condition->accept(*this);
}
