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

void FunctionMerger::visitUnboxInstr(UnboxInstr& instr) {
}

void FunctionMerger::visitToDoubleInstr(ToDoubleInstr& instr) {
}

void FunctionMerger::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
}

void FunctionMerger::visitAddInstr(AddInstr& instr) {
}

void FunctionMerger::visitStrAddInstr(StrAddInstr& instr) {
}

void FunctionMerger::visitIntAddInstr(IntAddInstr& instr) {
}

void FunctionMerger::visitDoubleAddInstr(DoubleAddInstr& instr) {
}

void FunctionMerger::visitSubInstr(SubInstr& instr) {
}

void FunctionMerger::visitIntSubInstr(IntSubInstr& instr) {
}

void FunctionMerger::visitDoubleSubInstr(DoubleSubInstr& instr) {
}

void FunctionMerger::visitMulInstr(MulInstr& instr) {
}

void FunctionMerger::visitIntMulInstr(IntMulInstr& instr) {
}

void FunctionMerger::visitDoubleMulInstr(DoubleMulInstr& instr) {
}

void FunctionMerger::visitDivInstr(DivInstr& instr) {
}

void FunctionMerger::visitNotInstr(NotInstr& instr) {
}

void FunctionMerger::visitCmpEqInstr(CmpEqInstr& instr) {
}

void FunctionMerger::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
}

void FunctionMerger::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
}

void FunctionMerger::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void FunctionMerger::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
}

void FunctionMerger::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
}

void FunctionMerger::visitCmpGtInstr(CmpGtInstr& instr) {
}

void FunctionMerger::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
}

void FunctionMerger::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
}

void FunctionMerger::visitCmpGteInstr(CmpGteInstr& instr) {
}

void FunctionMerger::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
}

void FunctionMerger::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
}

void FunctionMerger::visitCmpLtInstr(CmpLtInstr& instr) {
}

void FunctionMerger::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
}

void FunctionMerger::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
}

void FunctionMerger::visitCmpLteInstr(CmpLteInstr& instr) {
}

void FunctionMerger::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
}

void FunctionMerger::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
}

void FunctionMerger::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
}

void FunctionMerger::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
}

void FunctionMerger::visitListAccessInstr(ListAccessInstr& instr) {
}

void FunctionMerger::visitListStoreInstr(ListStoreInstr& instr) {
}

void FunctionMerger::visitListInitInstr(ListInitInstr& instr) {
}

void FunctionMerger::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
}

void FunctionMerger::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
}

void FunctionMerger::visitObjectAccessInstr(ObjectAccessInstr& instr) {
}

void FunctionMerger::visitObjectStoreInstr(ObjectStoreInstr& instr) {
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
