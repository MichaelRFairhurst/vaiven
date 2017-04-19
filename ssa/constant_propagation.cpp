#include "constant_propagation.h"

#include "../std.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void ConstantPropagator::visitPhiInstr(PhiInstr& instr) {
}

void ConstantPropagator::visitArgInstr(ArgInstr& instr) {
}

void ConstantPropagator::visitConstantInstr(ConstantInstr& instr) {
}

void ConstantPropagator::visitCallInstr(CallInstr& instr) {
}

void ConstantPropagator::visitTypecheckInstr(TypecheckInstr& instr) {
}

void ConstantPropagator::visitBoxInstr(BoxInstr& instr) {
}

void ConstantPropagator::visitUnboxInstr(UnboxInstr& instr) {
}

void ConstantPropagator::visitToDoubleInstr(ToDoubleInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT
      && instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    instr.replaceUsagesWith(instr.inputs[0]);
  } else if (instr.inputs[0]->tag == INSTR_CONSTANT
      && instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT) {
    double newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
}

void ConstantPropagator::replaceWithConstant(Instruction& instr, Value newVal) {
  Instruction* newConstant = new ConstantInstr(newVal);
  replace(instr, newConstant);
}

void append(Instruction& instr, Instruction* toAppend) {
  Instruction* next = instr.next;
  toAppend->next = next;
  instr.next = toAppend;
}

void ConstantPropagator::replace(Instruction& oldInstr, Instruction* newInstr) {
  if (oldInstr.usages.size() == 0) {
    return;
  }

  // add the constant that will replace this to the instruction list. Let dead code
  // elim remove this node later.
  oldInstr.append(newInstr);

  oldInstr.replaceUsagesWith(newInstr);
  performedWork = true;
}

bool ConstantPropagator::isConstantBinInstruction(Instruction& instr) {
  return instr.inputs[0]->tag == INSTR_CONSTANT
      && instr.inputs[1]->tag == INSTR_CONSTANT;
}

bool ConstantPropagator::isConstantBinIntInstruction(Instruction& instr) {
  return isConstantBinInstruction(instr)
      && instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      && instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT;
}

void ConstantPropagator::visitAddInstr(AddInstr& instr) {
  // TODO can this have constant values?
}

void ConstantPropagator::visitStrAddInstr(StrAddInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    Value valA = static_cast<ConstantInstr*>(instr.inputs[0])->val;
    Value valB = static_cast<ConstantInstr*>(instr.inputs[1])->val;
    string strA = ((GcableString*) valA.getPtr())->str;
    string strB = ((GcableString*) valB.getPtr())->str;

    // TODO generate a constant that's collected properly
    replaceWithConstant(instr, Value(new GcableString(strA + strB)));
  }
}

void ConstantPropagator::visitIntAddInstr(IntAddInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        + static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleAddInstr(DoubleAddInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    double newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        + static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitSubInstr(SubInstr& instr) {
  // TODO can this have constant values?
}

void ConstantPropagator::visitIntSubInstr(IntSubInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        - static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleSubInstr(DoubleSubInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    double newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        - static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitMulInstr(MulInstr& instr) {
  // TODO can this have constant values?
}

void ConstantPropagator::visitIntMulInstr(IntMulInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        * static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleMulInstr(DoubleMulInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    double newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        * static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDivInstr(DivInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    double newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        / static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitNotInstr(NotInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getBool();

    replaceWithConstant(instr, Value(!newval));
  }
}

void ConstantPropagator::visitCmpEqInstr(CmpEqInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = cmpUnboxed(static_cast<ConstantInstr*>(instr.inputs[0])->val,
        static_cast<ConstantInstr*>(instr.inputs[1])->val);

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        == static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        == static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = inverseCmpUnboxed(static_cast<ConstantInstr*>(instr.inputs[0])->val,
        static_cast<ConstantInstr*>(instr.inputs[1])->val);

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        != static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        != static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitCmpGtInstr(CmpGtInstr& instr) {
  // TODO can these have constant values?
}

void ConstantPropagator::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        > static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        > static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitCmpGteInstr(CmpGteInstr& instr) {
  // TODO can these have constant values?
}

void ConstantPropagator::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        >= static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        >= static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitCmpLtInstr(CmpLtInstr& instr) {
  // TODO can these have constant values?
}

void ConstantPropagator::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        < static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        < static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitCmpLteInstr(CmpLteInstr& instr) {
  // TODO can these have constant values?
}

void ConstantPropagator::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        <= static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
  if (isConstantBinInstruction(instr)) {
    bool newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getDouble()
        <= static_cast<ConstantInstr*>(instr.inputs[1])->val.getDouble();

    replaceWithConstant(instr, Value(newval));
  }
}

void ConstantPropagator::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
  // todo track "constant" lists/objects
}

void ConstantPropagator::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
  // todo track "constant" lists/objects
}

void ConstantPropagator::visitListAccessInstr(ListAccessInstr& instr) {
  // todo track "constant" lists
}

void ConstantPropagator::visitListStoreInstr(ListStoreInstr& instr) {
  // todo track "constant" lists
}

void ConstantPropagator::visitListInitInstr(ListInitInstr& instr) {
  // todo track "constant" lists
}

void ConstantPropagator::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  // todo track "constant" objects
}

void ConstantPropagator::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
  // todo track "constant" objects
}

void ConstantPropagator::visitObjectAccessInstr(ObjectAccessInstr& instr) {
  // todo track "constant" objects
}

void ConstantPropagator::visitObjectStoreInstr(ObjectStoreInstr& instr) {
  // todo track "constant" objects
}

void ConstantPropagator::visitErrInstr(ErrInstr& instr) {
}

void ConstantPropagator::visitRetInstr(RetInstr& instr) {
}

void ConstantPropagator::visitJmpCcInstr(JmpCcInstr& instr) {
}
