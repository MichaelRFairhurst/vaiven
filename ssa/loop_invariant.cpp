#include "loop_invariant.h"

using namespace vaiven::ssa;

void LoopInvariantCodeMover::visitPureInstruction(Instruction& instr) {
  for (vector<Instruction*>::iterator it = instr.inputs.begin();
      it != instr.inputs.end();
      ++it) {
    Block* block = (*it)->block;
    set<Block*> headerPredecessors = currentLoopHeader->allPredecessors;

    if (block != currentLoopHeader
      && headerPredecessors.find(block) == headerPredecessors.end()) {
      // abort
      return;
    }
  }

  for (set<Instruction*>::iterator it = instr.usages.begin();
      it != instr.usages.end();
      ++it) {
    if ((*it)->tag == INSTR_PHI) {
      // can't move values used by PHIs trivially
      return;
    }
  }

  if (lastInstr == NULL) {
    curBlock->head.release();
    curBlock->head.reset(instr.next);
  } else {
    lastInstr->next = instr.next;
  }

  instr.next = NULL;

  if (currentLoopHeader->head.get() == NULL) {
    currentLoopHeader->head.reset(&instr);
    writePoint = &instr;
  } else {
    writePoint->next = &instr;
  }
}

void LoopInvariantCodeMover::visitPhiInstr(PhiInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitArgInstr(ArgInstr& instr) {
}

void LoopInvariantCodeMover::visitConstantInstr(ConstantInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCallInstr(CallInstr& instr) {
  if (instr.func.isNative && instr.func.isPure) {
    visitPureInstruction(instr);
  }
}

void LoopInvariantCodeMover::visitTypecheckInstr(TypecheckInstr& instr) {
}

void LoopInvariantCodeMover::visitBoxInstr(BoxInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitAddInstr(AddInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitStrAddInstr(StrAddInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitIntAddInstr(IntAddInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitSubInstr(SubInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitMulInstr(MulInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDivInstr(DivInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitNotInstr(NotInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpEqInstr(CmpEqInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpIneqInstr(CmpIneqInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpGtInstr(CmpGtInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpGteInstr(CmpGteInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpLtInstr(CmpLtInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpLteInstr(CmpLteInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
  // not pure
}

void LoopInvariantCodeMover::visitListAccessInstr(ListAccessInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitListStoreInstr(ListStoreInstr& instr) {
  // not pure
}

void LoopInvariantCodeMover::visitListInitInstr(ListInitInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
  // not pure
}

void LoopInvariantCodeMover::visitObjectAccessInstr(ObjectAccessInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitObjectStoreInstr(ObjectStoreInstr& instr) {
  // not pure
}

void LoopInvariantCodeMover::visitErrInstr(ErrInstr& instr) {
  // not pure
}

void LoopInvariantCodeMover::visitRetInstr(RetInstr& instr) {
  // not pure
}

void LoopInvariantCodeMover::visitJmpCcInstr(JmpCcInstr& instr) {
  // no value in moving this
}

void LoopInvariantCodeMover::visitBlock(Block& block) {
  Block* prevBlock = curBlock;
  curBlock = &block;
  if (block.loopHeaderWithBlocks.size() > 0) {
    if (block.immPredecessors.size() == 1) {
      currentLoopHeader = prevBlock;
    } else {
      currentLoopHeader = new Block();
      currentLoopHeader->next.reset(prevBlock->next.release());
      prevBlock->next.reset(currentLoopHeader);
      currentLoopHeader->exits.push_back(unique_ptr<BlockExit>(new UnconditionalBlockExit(&block)));

      for (set<Block*>::iterator it = block.immPredecessors.begin();
          it != block.immPredecessors.end();
          ++it) {
        for (vector<unique_ptr<BlockExit>>::iterator exIt = (*it)->exits.begin();
            exIt != (*it)->exits.end();
            ++exIt) {
          if ((*exIt)->toGoTo == &block) {
            (*exIt)->toGoTo = currentLoopHeader;
          }
        }
      }

      currentLoopHeader->immPredecessors = block.immPredecessors;
      currentLoopHeader->allPredecessors = block.allPredecessors;
      currentLoopHeader->dominators = block.dominators;
      block.immPredecessors.clear();
      block.immPredecessors.insert(currentLoopHeader);
      block.allPredecessors.insert(currentLoopHeader);
      block.dominators.insert(currentLoopHeader);

      // mostly ok, but later blocks won't have the new header in allPredecessors
      requiresRebuildDominators = true;
    }

    writePoint = currentLoopHeader->head.get();
    while (writePoint != NULL && writePoint->next != NULL) {
      writePoint = writePoint->next;
    }

    for (set<Block*>::iterator it = block.loopHeaderWithBlocks.begin();
        it != block.loopHeaderWithBlocks.end();
        ++it) {
      curBlock = &**it;
      lastInstr = NULL;
      Instruction* next = curBlock->head.get();
      while (next != NULL) {
        next->accept(*this);
        lastInstr = next;
        next = next->next;
      }
    }
  }

  if (block.next != NULL) {
    block.next->accept(*this);
  }
}
