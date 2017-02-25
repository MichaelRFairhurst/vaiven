#include "constant_inliner.h"

using namespace vaiven::ssa;

void ConstantInliner::visitPhiInstr(PhiInstr& instr) {
}

void ConstantInliner::visitArgInstr(ArgInstr& instr) {
}

void ConstantInliner::visitConstantInstr(ConstantInstr& instr) {
}

void ConstantInliner::visitCallInstr(CallInstr& instr) {
}

void ConstantInliner::visitTypecheckInstr(TypecheckInstr& instr) {
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
}

void ConstantInliner::visitAddInstr(AddInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
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
}

void ConstantInliner::visitMulInstr(MulInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDivInstr(DivInstr& instr) {
}

void ConstantInliner::visitNotInstr(NotInstr& instr) {
}

void ConstantInliner::visitCmpEqInstr(CmpEqInstr& instr) {
}

void ConstantInliner::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void ConstantInliner::visitCmpGtInstr(CmpGtInstr& instr) {
}

void ConstantInliner::visitCmpGteInstr(CmpGteInstr& instr) {
}

void ConstantInliner::visitCmpLtInstr(CmpLtInstr& instr) {
}

void ConstantInliner::visitCmpLteInstr(CmpLteInstr& instr) {
}

void ConstantInliner::visitErrInstr(ErrInstr& instr) {
}

void ConstantInliner::visitRetInstr(RetInstr& instr) {
}
