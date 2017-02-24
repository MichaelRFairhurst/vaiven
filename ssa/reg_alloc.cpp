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
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitArgInstr(ArgInstr& instr) {
  instr.out = cc.newUInt64();
  cc.setArg(instr.argi, instr.out);
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitConstantInstr(ConstantInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitCallInstr(CallInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitTypecheckInstr(TypecheckInstr& instr) {
  reuseInputRegIfPossible(instr);
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitBoxInstr(BoxInstr& instr) {
  reuseInputRegIfPossible(instr);
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitAddInstr(AddInstr& instr) {
  reuseInputRegIfPossible(instr);
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitSubInstr(SubInstr& instr) {
  reuseInputRegIfPossible(instr);
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitMulInstr(MulInstr& instr) {
  reuseInputRegIfPossible(instr);
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitDivInstr(DivInstr& instr) {
  reuseInputRegIfPossible(instr);
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitNotInstr(NotInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitCmpEqInstr(CmpEqInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitCmpIneqInstr(CmpIneqInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitCmpGtInstr(CmpGtInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitCmpGteInstr(CmpGteInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitCmpLtInstr(CmpLtInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitCmpLteInstr(CmpLteInstr& instr) {
  instr.out = cc.newUInt64();
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitErrInstr(ErrInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void RegAlloc::visitRetInstr(RetInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}
