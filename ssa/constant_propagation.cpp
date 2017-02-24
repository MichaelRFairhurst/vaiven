#include "constant_propagation.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void ConstantPropagator::visitPhiInstr(PhiInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitArgInstr(ArgInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitConstantInstr(ConstantInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitCallInstr(CallInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitTypecheckInstr(TypecheckInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitBoxInstr(BoxInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
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

bool ConstantPropagator::isConstantBinIntInstruction(Instruction& instr) {
  return instr.inputs[0]->tag == INSTR_CONSTANT
      && instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->type == VAIVEN_STATIC_TYPE_BOOL
      && instr.inputs[1]->type == VAIVEN_STATIC_TYPE_BOOL;
}

void ConstantPropagator::visitAddInstr(AddInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        + static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitSubInstr(SubInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        - static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitMulInstr(MulInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        * static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitDivInstr(DivInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        / static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitNotInstr(NotInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getBool();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitCmpEqInstr(CmpEqInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getRaw()
        == static_cast<ConstantInstr*>(instr.inputs[1])->val.getRaw();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getRaw()
        != static_cast<ConstantInstr*>(instr.inputs[1])->val.getRaw();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitCmpGtInstr(CmpGtInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        > static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitCmpGteInstr(CmpGteInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        >= static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitCmpLtInstr(CmpLtInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        < static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitCmpLteInstr(CmpLteInstr& instr) {
  if (isConstantBinIntInstruction(instr)) {
    int newval = static_cast<ConstantInstr*>(instr.inputs[0])->val.getInt()
        <= static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt();

    replaceWithConstant(instr, Value(newval));
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitErrInstr(ErrInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void ConstantPropagator::visitRetInstr(RetInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}