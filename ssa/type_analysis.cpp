#include "type_analysis.h"

using namespace vaiven::ssa;

void TypeAnalysis::emit(Instruction* instr) {
  if (lastInstr == NULL) {
    instr->next = curBlock->head.release();;
    curBlock->head.reset(instr);
  } else {
    lastInstr->append(instr);
  }
}

void TypeAnalysis::box(Instruction** input, Instruction* instr) {
  // Always generate a box instruction now, though it may be optimized out
  // later if it isn't needed or if the instruction can be assembled boxed.
  Instruction* curInput = *input;
  BoxInstr* boxInstr = new BoxInstr(curInput);
  emit(boxInstr);
  curInput->usages.erase(instr);
  *input = boxInstr;
  boxInstr->usages.insert(instr);
}

void TypeAnalysis::visitPhiInstr(PhiInstr& instr) {
  VaivenStaticType commonType = instr.inputs[0]->type;
  for (vector<Instruction*>::iterator it = instr.inputs.begin() + 1;
      it != instr.inputs.end();
      ++it) {
    if ((*it)->type != commonType) {
      commonType = VAIVEN_STATIC_TYPE_UNKNOWN;
      break;
    }
  }

  instr.type = commonType;
  if (commonType == VAIVEN_STATIC_TYPE_UNKNOWN) {
    for (vector<Instruction*>::iterator it = instr.inputs.begin();
        it != instr.inputs.end();
        ++it) {
      if (!(*it)->isBoxed) {
        box(&*it, &instr);
      }
    }
    instr.isBoxed = true;
  } else {
    // TODO once we support doubles, this must box the required doubles
    instr.isBoxed = false;
  }
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
      box(&*it, &instr);
    }
  }
}

void TypeAnalysis::visitTypecheckInstr(TypecheckInstr& instr) {
}

void TypeAnalysis::visitBoxInstr(BoxInstr& instr) {
}

void TypeAnalysis::typecheckInput(Instruction& instr, VaivenStaticType expectedType, int input) {
  if (instr.inputs[input]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    // TODO cache this so we don't have to detect it with CSE
    TypecheckInstr* checkInstr = new TypecheckInstr(instr.inputs[input], expectedType);
    emit(checkInstr);
    // edge case: x + x. But this will be solved later in this function.
    instr.inputs[input]->usages.erase(&instr);
    instr.inputs[input]->usages.insert(checkInstr);
    instr.inputs[input] = checkInstr;
  } else if (instr.inputs[input]->type != expectedType) {
    if (expectedType == VAIVEN_STATIC_TYPE_INT) {
      emit(new ErrInstr(EXPECTED_INT));
    } else if (expectedType == VAIVEN_STATIC_TYPE_BOOL) {
      emit(new ErrInstr(EXPECTED_BOOL));
    } else if (expectedType == VAIVEN_STATIC_TYPE_STRING) {
      emit(new ErrInstr(EXPECTED_STR));
    }
  }
}

void TypeAnalysis::visitBinIntInstruction(Instruction& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_INT, 0);
  typecheckInput(instr, VAIVEN_STATIC_TYPE_INT, 1);
}

void TypeAnalysis::visitAddInstr(AddInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT) {
    instr.append(new IntAddInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
  } else if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING) {
    instr.append(new StrAddInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT) {
    instr.append(new IntAddInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    instr.append(new StrAddInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
  }
}

void TypeAnalysis::visitIntAddInstr(IntAddInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitStrAddInstr(StrAddInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_STRING, 0);
  typecheckInput(instr, VAIVEN_STATIC_TYPE_STRING, 1);
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
  typecheckInput(instr, VAIVEN_STATIC_TYPE_BOOL, 0);
}

void TypeAnalysis::visitCmpEqInstr(CmpEqInstr& instr) {
  // When one type is unknown, the other must be unknown or boxed
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && !instr.inputs[1]->isBoxed) {
    box(&instr.inputs[1], &instr);
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && !instr.inputs[0]->isBoxed) {
    box(&instr.inputs[0], &instr);
  }
}

void TypeAnalysis::visitCmpIneqInstr(CmpIneqInstr& instr) {
  // When one type is unknown, the other must be unknown or boxed
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && !instr.inputs[1]->isBoxed) {
    box(&instr.inputs[1], &instr);
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && !instr.inputs[0]->isBoxed) {
    box(&instr.inputs[0], &instr);
  }
}

void TypeAnalysis::visitCmpGtInstr(CmpGtInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitCmpGteInstr(CmpGteInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitCmpLtInstr(CmpLtInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitCmpLteInstr(CmpLteInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitErrInstr(ErrInstr& instr) {
}

void TypeAnalysis::visitRetInstr(RetInstr& instr) {
  if (!instr.inputs[0]->isBoxed) {
    box(&instr.inputs[0], &instr);
  }
}

void TypeAnalysis::visitJmpCcInstr(JmpCcInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_BOOL, 0);
}
