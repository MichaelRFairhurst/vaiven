#include "reg_alloc.h"

#include <iostream>

using namespace vaiven::ssa;
using namespace std;

void RegAlloc::reuseInputRegIfPossible(Instruction& instr) {
  if (instr.inputs.size() > 0 && instr.inputs[0]->usages.size() == 1) {
    instr.out = instr.inputs[0]->out;
  } else {
    instr.out = cc.newUInt64();
  }
}

void RegAlloc::visitPhiInstr(PhiInstr& instr) {
  reuseInputRegIfPossible(instr);
}

void RegAlloc::visitArgInstr(ArgInstr& instr) {
  instr.out = cc.newUInt64();
  cc.setArg(instr.argi, instr.out);
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
