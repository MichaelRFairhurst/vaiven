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
  // !true or !false is always reduced by constant propagation
}

void ConstantInliner::visitCmpEqInstr(CmpEqInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  if (instr.inputs[1]->tag == INSTR_CONSTANT
      && (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
          || instr.inputs[0]->type == VAIVEN_STATIC_TYPE_BOOL)) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constI32Rhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    // TODO do I have to replace with a boxed constant?
    instr.inputs[1]->isBoxed = true;
  }
}

void ConstantInliner::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  if (instr.inputs[1]->tag == INSTR_CONSTANT
      && (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
          || instr.inputs[0]->type == VAIVEN_STATIC_TYPE_BOOL)) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constI32Rhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    // TODO do I have to replace with a boxed constant?
    instr.inputs[1]->isBoxed = true;
  }
}

void ConstantInliner::visitCmpGtInstr(CmpGtInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitCmpGteInstr(CmpGteInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitCmpLtInstr(CmpLtInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitCmpLteInstr(CmpLteInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitErrInstr(ErrInstr& instr) {
}

void ConstantInliner::visitRetInstr(RetInstr& instr) {
}

void ConstantInliner::visitJmpCcInstr(JmpCcInstr& instr) {
}
