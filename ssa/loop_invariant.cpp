#include "loop_invariant.h"

using namespace vaiven::ssa;

void LoopInvariantCodeMover::visitPureInstruction(Instruction& instr) {
  for (vector<Instruction*>::iterator it = instr.inputs.begin();
      it != instr.inputs.end();
      ++it) {
    Block* block = (*it)->block;
    set<Block*> headerDominators = currentPreHeader->dominators;

    if (block != currentPreHeader
      && headerDominators.find(block) == headerDominators.end()) {
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

  if (currentPreHeader->head.get() == NULL) {
    currentPreHeader->head.reset(&instr);
    instr.block = currentPreHeader;
    writePoint = &instr;
  } else {
    writePoint->append(&instr);
    writePoint = &instr;
  }

  performedWork = true;
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

void LoopInvariantCodeMover::visitUnboxInstr(UnboxInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitToDoubleInstr(ToDoubleInstr& instr) {
  // not pure, can throw
}

void LoopInvariantCodeMover::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
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

void LoopInvariantCodeMover::visitDoubleAddInstr(DoubleAddInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitSubInstr(SubInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitIntSubInstr(IntSubInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleSubInstr(DoubleSubInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitMulInstr(MulInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitIntMulInstr(IntMulInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleMulInstr(DoubleMulInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDivInstr(DivInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitNotInstr(NotInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpEqInstr(CmpEqInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpIneqInstr(CmpIneqInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpGtInstr(CmpGtInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpGteInstr(CmpGteInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpLtInstr(CmpLtInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitCmpLteInstr(CmpLteInstr& instr) {
  // not pure, can throw :(
}

void LoopInvariantCodeMover::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
  visitPureInstruction(instr);
}

void LoopInvariantCodeMover::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
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
    set<Block*> incomingForwardEdges;
    for (set<Block*>::iterator it = block.immPredecessors.begin();
        it != block.immPredecessors.end();
        ++it) {
      if ((*it)->allPredecessors.find(&block) != (*it)->allPredecessors.end()) {
        continue;
      }

      incomingForwardEdges.insert(*it);
    }

    if (incomingForwardEdges.size() == 1) {
      currentPreHeader = prevBlock;
    } else {
      currentPreHeader = new Block();
      currentPreHeader->next.reset(prevBlock->next.release());
      prevBlock->next.reset(currentPreHeader);
      currentPreHeader->exits.push_back(unique_ptr<BlockExit>(new UnconditionalBlockExit(&block)));

      for (set<Block*>::iterator it = incomingForwardEdges.begin();
          it != incomingForwardEdges.end();
          ++it) {
        for (vector<unique_ptr<BlockExit>>::iterator exIt = (*it)->exits.begin();
            exIt != (*it)->exits.end();
            ++exIt) {
          if ((*exIt)->toGoTo == &block) {
            (*exIt)->toGoTo = currentPreHeader;
          }
        }
      }

      currentPreHeader->immPredecessors = incomingForwardEdges;
      currentPreHeader->allPredecessors = block.allPredecessors;
      currentPreHeader->dominators = block.dominators;
      block.immPredecessors.erase(incomingForwardEdges.begin(), incomingForwardEdges.end());
      block.immPredecessors.insert(currentPreHeader);
      block.allPredecessors.insert(currentPreHeader);
      block.dominators.insert(currentPreHeader);

      // mostly ok, but later blocks won't have the new pre-header in
      // allPredecessors / dominators
      requiresRebuildDominators = true;
    }

    writePoint = currentPreHeader->head.get();
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
