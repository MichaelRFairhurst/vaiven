#include "unused_code.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void UnusedCodeEliminator::remove(Instruction* instr) {
  if (last != NULL) {
    last->next = instr->next;
  }
  delete instr;
  performedWork = true;
}

void UnusedCodeEliminator::visitPureInstr(Instruction& instr) {
  // save next before instr is maybe freed!!
  Instruction* next = instr.next;
  if (instr.usages.size() == 0) {
     remove(&instr);
  } else {
    last = &instr;
    if (start == NULL) start = &instr;
  }
  if (next != NULL) next->accept(*this);
}

void UnusedCodeEliminator::visitImpureInstr(Instruction& instr) {
  last = &instr;
  if (start == NULL) start = &instr;
  if (instr.next != NULL) instr.next->accept(*this);
}

void UnusedCodeEliminator::visitPhiInstr(PhiInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitArgInstr(ArgInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitConstantInstr(ConstantInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCallInstr(CallInstr& instr) {
  visitImpureInstr(instr);
}

void UnusedCodeEliminator::visitTypecheckInstr(TypecheckInstr& instr) {
  visitImpureInstr(instr);
}

void UnusedCodeEliminator::visitBoxInstr(BoxInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitAddInstr(AddInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitSubInstr(SubInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitMulInstr(MulInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitDivInstr(DivInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitNotInstr(NotInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpEqInstr(CmpEqInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpIneqInstr(CmpIneqInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpGtInstr(CmpGtInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpGteInstr(CmpGteInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpLtInstr(CmpLtInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpLteInstr(CmpLteInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitErrInstr(ErrInstr& instr) {
  Instruction* deleteFrom = instr.next;
  instr.next = NULL;

  // TODO free multiple instructions in a row right
  //while (deleteFrom != NULL) {
  //  Instruction* next = deleteFrom->next;
  //  delete deleteFrom;
  //  deleteFrom = next;
  //}
}

void UnusedCodeEliminator::visitRetInstr(RetInstr& instr) {
  Instruction* deleteFrom = instr.next;
  instr.next = NULL;

  // TODO free multiple instructions in a row right
  //while (deleteFrom != NULL) {
  //  Instruction* next = deleteFrom->next;
  //  delete deleteFrom;
  //  deleteFrom = next;
  //}
}
