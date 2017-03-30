#ifndef VAIVEN_VISITOR_HEADER_SSA_CFG
#define VAIVEN_VISITOR_HEADER_SSA_CFG

#include <memory>

#include "ssa.h"

using std::unique_ptr;

namespace vaiven { namespace ssa {

class BlockExit;

class Block {
  public:
  Block() : head(), next() {};

  asmjit::Label label;
  unique_ptr<Instruction> head;
  unique_ptr<Block> next;

  // don't use unordered set, harder to intersect
  set<Block*> immPredecessors;
  set<Block*> allPredecessors;
  set<Block*> backReferences;
  set<Block*> dominators;

  set<Block*> loopHeaderWithBlocks;
  
  vector<unique_ptr<BlockExit>> exits;

  void accept(SsaVisitor& visitor) {
    visitor.visitBlock(*this);
  }
};

enum BlockExitType {
  BLOCK_EXIT_UNCONDITIONAL,
  BLOCK_EXIT_CONDITIONAL,
};

class BlockExit {
  public:
  BlockExit(BlockExitType tag, Block* toGoTo) : tag(tag), toGoTo(toGoTo) {};
  virtual void accept(SsaVisitor& visitor)=0;
  virtual ~BlockExit() {};
  BlockExitType tag;
  Block* toGoTo;
};

class UnconditionalBlockExit : public BlockExit {
  public:
  UnconditionalBlockExit(Block* toGoTo) : BlockExit(BLOCK_EXIT_UNCONDITIONAL, toGoTo) {};
  ~UnconditionalBlockExit() {};
  virtual void accept(SsaVisitor& visitor) {
    visitor.visitUnconditionalBlockExit(*this);
  }
};

class ConditionalBlockExit : public BlockExit {
  public:
  ConditionalBlockExit(Instruction* condition, Block* toGoTo)
      : condition(condition), BlockExit(BLOCK_EXIT_CONDITIONAL, toGoTo) {};
  unique_ptr<Instruction> condition;
  ~ConditionalBlockExit() {};

  virtual void accept(SsaVisitor& visitor) {
    visitor.visitConditionalBlockExit(*this);
  }
};

} }

#endif
