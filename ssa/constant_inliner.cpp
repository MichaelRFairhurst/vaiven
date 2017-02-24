#include "constant_inliner.h"

using namespace vaiven::ssa;

void ConstantInliner::visitPhiInstr(PhiInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitArgInstr(ArgInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitConstantInstr(ConstantInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitCallInstr(CallInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitTypecheckInstr(TypecheckInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitBoxInstr(BoxInstr& instr) {
  // create a boxed constant where needed/useful
  if (instr.inputs[0]->tag == INSTR_CONSTANT && !instr.inputs[0]->isBoxed) {
    Value val = static_cast<ConstantInstr*>(instr.inputs[0])->val;
    ConstantInstr* boxed = new ConstantInstr(val);
    boxed->isBoxed = true;
    instr.append(boxed);
    instr.replaceUsagesWith(boxed);
  }
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitAddInstr(AddInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitSubInstr(SubInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt();
    instr.hasConstLhs = true;
    instr.constLhs = val;
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = instr.inputs[1];
    instr.inputs.pop_back();
  }
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitMulInstr(MulInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitDivInstr(DivInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitNotInstr(NotInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitCmpEqInstr(CmpEqInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitCmpGtInstr(CmpGtInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitCmpGteInstr(CmpGteInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitCmpLtInstr(CmpLtInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitCmpLteInstr(CmpLteInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitErrInstr(ErrInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantInliner::visitRetInstr(RetInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}
