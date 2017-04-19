#include "constant_inliner.h"

#include "../heap.h"

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

  // these can also be generated in boxed formats
  if (instr.inputs[0]->tag == INSTR_CONSTANT
      || instr.inputs[0]->tag == INSTR_NOT
      || instr.inputs[0]->tag == INSTR_INT_CMPEQ
      || instr.inputs[0]->tag == INSTR_INT_CMPINEQ
      || instr.inputs[0]->tag == INSTR_INT_CMPGT
      || instr.inputs[0]->tag == INSTR_INT_CMPGTE
      || instr.inputs[0]->tag == INSTR_INT_CMPLT
      || instr.inputs[0]->tag == INSTR_INT_CMPLTE) {
    instr.inputs[0]->isBoxed = true;
    instr.replaceUsagesWith(instr.inputs[0]);
    return;
  }
}

void ConstantInliner::visitUnboxInstr(UnboxInstr& instr) {
}

void ConstantInliner::visitToDoubleInstr(ToDoubleInstr& instr) {
}

void ConstantInliner::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
}

void ConstantInliner::visitAddInstr(AddInstr& instr) {
}

void ConstantInliner::visitStrAddInstr(StrAddInstr& instr) {
}

void ConstantInliner::visitIntAddInstr(IntAddInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDoubleAddInstr(DoubleAddInstr& instr) {
}

void ConstantInliner::visitSubInstr(SubInstr& instr) {
}

void ConstantInliner::visitIntSubInstr(IntSubInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt();
    instr.hasConstLhs = true;
    instr.constLhs = val;
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = instr.inputs[1];
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDoubleSubInstr(DoubleSubInstr& instr) {
}

void ConstantInliner::visitMulInstr(MulInstr& instr) {
}

void ConstantInliner::visitIntMulInstr(IntMulInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDoubleMulInstr(DoubleMulInstr& instr) {
}

void ConstantInliner::visitDivInstr(DivInstr& instr) {
}

void ConstantInliner::visitNotInstr(NotInstr& instr) {
  // !true or !false is always reduced by constant propagation
}

void ConstantInliner::visitCmpEqInstr(CmpEqInstr& instr) {
}

void ConstantInliner::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
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

void ConstantInliner::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
}

void ConstantInliner::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void ConstantInliner::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
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

void ConstantInliner::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
}

void ConstantInliner::visitCmpGtInstr(CmpGtInstr& instr) {
}

void ConstantInliner::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
}

void ConstantInliner::visitCmpGteInstr(CmpGteInstr& instr) {
}

void ConstantInliner::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
}

void ConstantInliner::visitCmpLtInstr(CmpLtInstr& instr) {
}

void ConstantInliner::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
}

void ConstantInliner::visitCmpLteInstr(CmpLteInstr& instr) {
}

void ConstantInliner::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    int val = static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();
    instr.hasConstRhs = true;
    instr.constRhs = val;
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs.pop_back();
  }
}

void ConstantInliner::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
}

void ConstantInliner::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
}

void ConstantInliner::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
}

void ConstantInliner::visitListAccessInstr(ListAccessInstr& instr) {
}

void ConstantInliner::visitListStoreInstr(ListStoreInstr& instr) {
}

void ConstantInliner::visitListInitInstr(ListInitInstr& instr) {
}

void ConstantInliner::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    GcableString* val = (GcableString*) static_cast<ConstantInstr*>(instr.inputs[1])->val.getPtr();
    // TODO don't leak this, but last beyond SSA phase
    string* property = new string(val->str);
    ObjectAccessInstr* newInstr =
      new ObjectAccessInstr(instr.inputs[0], property);
    instr.append(newInstr);
    instr.replaceUsagesWith(newInstr);
  }
}

void ConstantInliner::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    GcableString* val = (GcableString*) static_cast<ConstantInstr*>(instr.inputs[1])->val.getPtr();
    // TODO don't leak this, but last beyond SSA phase
    string* property = new string(val->str);
    ObjectStoreInstr* newInstr =
      new ObjectStoreInstr(instr.inputs[0], property, instr.inputs[2]);
    instr.append(newInstr);
    instr.replaceUsagesWith(newInstr);
    instr.safelyDeletable = true;
  }
}

void ConstantInliner::visitObjectAccessInstr(ObjectAccessInstr& instr) {
}

void ConstantInliner::visitObjectStoreInstr(ObjectStoreInstr& instr) {
}

void ConstantInliner::visitErrInstr(ErrInstr& instr) {
}

void ConstantInliner::visitRetInstr(RetInstr& instr) {
}

void ConstantInliner::visitJmpCcInstr(JmpCcInstr& instr) {
}
