#include "type_analysis.h"

using namespace vaiven::ssa;

void TypeAnalysis::emit(Instruction* instr) {
  if (lastInstr == NULL) {
    instr->next = curBlock->head;
    curBlock->head = instr;
  } else {
    lastInstr->append(instr);
  }
}

void TypeAnalysis::visitPhiInstr(PhiInstr& instr) {
}

void TypeAnalysis::visitArgInstr(ArgInstr& instr) {
}

void TypeAnalysis::visitConstantInstr(ConstantInstr& instr) {
}

void TypeAnalysis::visitCallInstr(CallInstr& instr) {
  for (vector<Instruction*>::iterator it = instr.inputs.begin();
      it != instr.inputs.end();
      ++it) {
    if (!(*it)->isBoxed) {
      BoxInstr* boxInstr = new BoxInstr((*it));
      emit(boxInstr);
      (*it)->usages.erase(&instr);
      (*it) = boxInstr;
      boxInstr->usages.insert(&instr);
    }
  }
}

void TypeAnalysis::visitTypecheckInstr(TypecheckInstr& instr) {
}

void TypeAnalysis::visitBoxInstr(BoxInstr& instr) {
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
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitSubInstr(SubInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitMulInstr(MulInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDivInstr(DivInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitNotInstr(NotInstr& instr) {
}

void TypeAnalysis::visitCmpEqInstr(CmpEqInstr& instr) {
}

void TypeAnalysis::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void TypeAnalysis::visitCmpGtInstr(CmpGtInstr& instr) {
}

void TypeAnalysis::visitCmpGteInstr(CmpGteInstr& instr) {
}

void TypeAnalysis::visitCmpLtInstr(CmpLtInstr& instr) {
}

void TypeAnalysis::visitCmpLteInstr(CmpLteInstr& instr) {
}

void TypeAnalysis::visitErrInstr(ErrInstr& instr) {
}

void TypeAnalysis::visitRetInstr(RetInstr& instr) {
  if (!instr.inputs[0]->isBoxed) {
    BoxInstr* boxInstr = new BoxInstr(instr.inputs[0]);
    emit(boxInstr);
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = boxInstr;
    boxInstr->usages.insert(&instr);
  }
}
