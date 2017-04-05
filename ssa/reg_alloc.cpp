#include "reg_alloc.h"

#include <iostream>

using namespace vaiven::ssa;
using namespace std;

void RegAlloc::reuseInputRegIfPossible(Instruction& instr) {
  if (instr.inputs.size() > 0 && instr.inputs[0]->usages.size() == 1) {
    // edge case: typecheck may have no other usages, but it shares a register
    // with something that may have other usages.
    if (instr.inputs[0]->tag == INSTR_TYPECHECK
        && instr.inputs[0]->inputs[0]->usages.size() > 1) {
      instr.out = cc.newUInt64();
      return;
    }
    instr.out = instr.inputs[0]->out;
  } else {
    instr.out = cc.newUInt64();
  }
}

void RegAlloc::visitPhiInstr(PhiInstr& instr) {
  instr.out = cc.newUInt64();

  // reuse any available input
  for (vector<Instruction*>::iterator it = instr.inputs.begin(); it != instr.inputs.end(); ++it) {
    if ((*it)->usages.size() == 1) {
      instr.out = (*it)->out;
      break;
    }
  }

  // all non-special, now-dead values can just output straight to the phi
  for (vector<Instruction*>::iterator it = instr.inputs.begin(); it != instr.inputs.end(); ++it) {
    if ((*it)->tag != INSTR_ARG && (*it)->tag != INSTR_PHI && (*it)->usages.size() == 1) {
      (*it)->out = instr.out;
    }
  }
}

void RegAlloc::visitArgInstr(ArgInstr& instr) {
  instr.out = argRegs[instr.argi];
}

void RegAlloc::visitConstantInstr(ConstantInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitCallInstr(CallInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitTypecheckInstr(TypecheckInstr& instr) {
  // we don't actually transform anything
  instr.out = instr.inputs[0]->out;
}

void RegAlloc::visitBoxInstr(BoxInstr& instr) {
  reuseInputRegIfPossible(instr);
}

void RegAlloc::visitAddInstr(AddInstr& instr) {
  // is a func call, can't reuse a register, it will be in rax no matter what
  instr.out = cc.newUInt64();
}

void RegAlloc::visitStrAddInstr(StrAddInstr& instr) {
  // is a func call, can't reuse a register, it will be in rax no matter what
  instr.out = cc.newUInt64();
}

void RegAlloc::visitIntAddInstr(IntAddInstr& instr) {
  // swap reg inputs if the lhs isn't reusable. Better chance of
  // avoiding a mov between virtual registers
  if (!instr.hasConstRhs && instr.inputs[0]->usages.size() > 1) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  reuseInputRegIfPossible(instr);
}

void RegAlloc::visitSubInstr(SubInstr& instr) {
  // swap reg inputs if the lhs isn't reusable. Better chance of
  // avoiding a mov between virtual registers
  if (!instr.hasConstLhs && instr.inputs[0]->usages.size() > 1) {
    std::swap(instr.inputs[0], instr.inputs[1]);
    // inverse subtraction has its own assembly
    instr.isInverse = true;
  }

  reuseInputRegIfPossible(instr);
}

void RegAlloc::visitMulInstr(MulInstr& instr) {
  // swap reg inputs if the lhs isn't reusable. Better chance of
  // avoiding a mov between virtual registers
  if (!instr.hasConstRhs && instr.inputs[0]->usages.size() > 1) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  reuseInputRegIfPossible(instr);
}

void RegAlloc::visitDivInstr(DivInstr& instr) {
  reuseInputRegIfPossible(instr);
}

void RegAlloc::visitNotInstr(NotInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitCmpEqInstr(CmpEqInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitCmpIneqInstr(CmpIneqInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitCmpGtInstr(CmpGtInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitCmpGteInstr(CmpGteInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitCmpLtInstr(CmpLtInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitCmpLteInstr(CmpLteInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
  instr.out = instr.inputs[2]->out;
}

void RegAlloc::visitListAccessInstr(ListAccessInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitListStoreInstr(ListStoreInstr& instr) {
  instr.out = instr.inputs[2]->out;
}

void RegAlloc::visitListInitInstr(ListInitInstr& instr) {
  // todo coalesce
  instr.out = cc.newUInt64();
}

void RegAlloc::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
  instr.out = instr.inputs[2]->out;
}

void RegAlloc::visitObjectAccessInstr(ObjectAccessInstr& instr) {
  instr.out = cc.newUInt64();
}

void RegAlloc::visitObjectStoreInstr(ObjectStoreInstr& instr) {
  instr.out = instr.inputs[1]->out;
}

void RegAlloc::visitErrInstr(ErrInstr& instr) {
}

void RegAlloc::visitRetInstr(RetInstr& instr) {
}

void RegAlloc::visitBlock(Block& block) {
  block.label = cc.newLabel();
  ForwardVisitor::visitBlock(block);
}

void RegAlloc::visitJmpCcInstr(JmpCcInstr& instr) {
  // this means we're jumping on a noncmp, like a var or func call result
  instr.out = instr.inputs[0]->out;
}
