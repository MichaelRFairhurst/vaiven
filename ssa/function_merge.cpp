#include "function_merge.h"

using namespace vaiven::ssa;

void FunctionMerger::visitPhiInstr(PhiInstr& instr) {
}

void FunctionMerger::visitArgInstr(ArgInstr& instr) {
}

void FunctionMerger::visitConstantInstr(ConstantInstr& instr) {
}

void FunctionMerger::visitCallInstr(CallInstr& instr) {
}

void FunctionMerger::visitTypecheckInstr(TypecheckInstr& instr) {
}

void FunctionMerger::visitBoxInstr(BoxInstr& instr) {
}

void FunctionMerger::visitAddInstr(AddInstr& instr) {
}

void FunctionMerger::visitSubInstr(SubInstr& instr) {
}

void FunctionMerger::visitMulInstr(MulInstr& instr) {
}

void FunctionMerger::visitDivInstr(DivInstr& instr) {
}

void FunctionMerger::visitNotInstr(NotInstr& instr) {
}

void FunctionMerger::visitCmpEqInstr(CmpEqInstr& instr) {
}

void FunctionMerger::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void FunctionMerger::visitCmpGtInstr(CmpGtInstr& instr) {
}

void FunctionMerger::visitCmpGteInstr(CmpGteInstr& instr) {
}

void FunctionMerger::visitCmpLtInstr(CmpLtInstr& instr) {
}

void FunctionMerger::visitCmpLteInstr(CmpLteInstr& instr) {
}

void FunctionMerger::visitErrInstr(ErrInstr& instr) {
}

void FunctionMerger::visitRetInstr(RetInstr& instr) {
  phi->inputs.push_back(instr.inputs[0]);
  instr.inputs[0]->usages.insert(phi);
  if (lastInstr != NULL) {
    lastInstr->next = NULL;
    delete &instr;
  } else {
    curBlock->head.reset(NULL);
  }
  curBlock->exits.clear();
  curBlock->exits.push_back(unique_ptr<BlockExit>(new UnconditionalBlockExit(returnPoint)));
}

void FunctionMerger::visitJmpCcInstr(JmpCcInstr& instr) {
}

void FunctionMerger::visitBlock(Block& block) {
  curBlock = &block;
  lastInstr = NULL;
  Instruction* next = block.head.get();
  while (next != NULL) {
    next->accept(*this);
    // special cases: next was deleted
    if (lastInstr != NULL && lastInstr->next != next) {
      next = lastInstr->next;
    } else if (lastInstr == NULL && block.head.get() != next) {
      next = block.head.get();
    } else {
      lastInstr = next;
      next = next->next;
    }
  }

  if (block.next != NULL) {
    block.next->accept(*this);
  }
}
