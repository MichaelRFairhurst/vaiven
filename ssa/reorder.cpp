#include "reorder.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void UnusedCodeEliminator::reorderBinary(Instruction& instr) {
}

void UnusedCodeEliminator::visitPhiInstr(PhiInstr& instr) {
}

void UnusedCodeEliminator::visitArgInstr(ArgInstr& instr) {
}

void UnusedCodeEliminator::visitConstantInstr(ConstantInstr& instr) {
}

void UnusedCodeEliminator::visitCallInstr(CallInstr& instr) {
}

void UnusedCodeEliminator::visitAddInstr(AddInstr& instr) {
  if (instr.constAdd == -1) {
    if (instr.inputs[0]->tag == INSTR_CONSTANT
        && instr.inputs[0]->tag != INSTR_CONSTANT) {
      std::swap(instr.inputs[0], instr.inputs[1]);
    }
  }
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
  visitImpureInstr(instr);
}

void UnusedCodeEliminator::visitRetInstr(RetInstr& instr) {
  visitImpureInstr(instr);
}

void UnusedCodeEliminator::visitJmpCcInstr(JmpCcInstr& instr) {
}
