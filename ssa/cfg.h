#ifndef VAIVEN_VISITOR_HEADER_SSA_CFG
#define VAIVEN_VISITOR_HEADER_SSA_CFG

#include <memory>

#include "ssa.h"

using std::unique_ptr;

namespace vaiven { namespace ssa {

class BlockExit;

class Block {
  public:
  Block() : head(NULL), next(NULL) {};

  asmjit::Label label;
  Instruction* head;
  Block* next;
  
  vector<unique_ptr<BlockExit>> exits;

  void accept(SsaVisitor& visitor) {
    visitor.visitBlock(*this);
  }
};

class BlockExit {
  public:
  virtual void accept(SsaVisitor& visitor)=0;
  Block* toGoTo;
};

class UnconditionalBlockExit : public BlockExit {
  public:
  virtual void accept(SsaVisitor& visitor) {
    visitor.visitUnconditionalBlockExit(*this);
  }
};

class ConditionalBlockExit : public BlockExit {
  public:
  ConditionalBlockExit(Instruction* condition) : condition(condition) {};
  Instruction* condition;

  virtual void accept(SsaVisitor& visitor) {
    visitor.visitConditionalBlockExit(*this);
  }
};

} }

#endif
