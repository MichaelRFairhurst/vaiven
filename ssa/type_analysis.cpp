#include "type_analysis.h"

using namespace vaiven::ssa;

void TypeAnalysis::emit(Instruction* instr) {
  if (last == NULL) {
    instr->next = start;
    start = instr;
  } else {
    last->append(instr);
  }
}

void TypeAnalysis::visitPhiInstr(PhiInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitArgInstr(ArgInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitConstantInstr(ConstantInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitCallInstr(CallInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitTypecheckInstr(TypecheckInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitBoxInstr(BoxInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitBinIntInstruction(Instruction& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    // TODO cache this so we don't have to detect it with CSE
    TypecheckInstr* checkInstr = new TypecheckInstr(instr.inputs[0], VAIVEN_STATIC_TYPE_INT);
    emit(checkInstr);
    // edge case: x + x. But this will be solved later in this function.
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = checkInstr;
  } else if (instr.inputs[0]->type != VAIVEN_STATIC_TYPE_INT) {
    emit(new ErrInstr());
  }

  if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    // TODO cache this so we don't have to detect it with CSE
    TypecheckInstr* checkInstr = new TypecheckInstr(instr.inputs[1], VAIVEN_STATIC_TYPE_INT);
    emit(checkInstr);
    // edge case: x + x, solved right now.
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs[1] = checkInstr;
  } else if (instr.inputs[1]->type != VAIVEN_STATIC_TYPE_INT) {
    emit(new ErrInstr());
  }
}

void TypeAnalysis::visitAddInstr(AddInstr& instr) {
  if (start == NULL) start = &instr;
  visitBinIntInstruction(instr);

  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitSubInstr(SubInstr& instr) {
  if (start == NULL) start = &instr;
  visitBinIntInstruction(instr);

  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitMulInstr(MulInstr& instr) {
  if (start == NULL) start = &instr;
  visitBinIntInstruction(instr);

  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitDivInstr(DivInstr& instr) {
  if (start == NULL) start = &instr;
  visitBinIntInstruction(instr);

  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitNotInstr(NotInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitCmpEqInstr(CmpEqInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitCmpGtInstr(CmpGtInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitCmpGteInstr(CmpGteInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitCmpLtInstr(CmpLtInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitCmpLteInstr(CmpLteInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitErrInstr(ErrInstr& instr) {
  if (start == NULL) start = &instr;
  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void TypeAnalysis::visitRetInstr(RetInstr& instr) {
  if (start == NULL) start = &instr;
  if (!instr.inputs[0]->isBoxed) {
    BoxInstr* boxInstr = new BoxInstr(instr.inputs[0]);
    emit(boxInstr);
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = boxInstr;
    boxInstr->usages.insert(&instr);
  }

  last = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}
