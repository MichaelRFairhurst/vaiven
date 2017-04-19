#include "type_analysis.h"

using namespace vaiven::ssa;

void TypeAnalysis::emit(Instruction* instr) {
  if (lastInstr == NULL) {
    instr->next = curBlock->head.release();;
    curBlock->head.reset(instr);
    instr->block = curBlock;
  } else {
    lastInstr->append(instr);
  }
}

void TypeAnalysis::box(Instruction** input, Instruction* instr) {
  // Always generate a box instruction now, though it may be optimized out
  // later if it isn't needed or if the instruction can be assembled boxed.
  Instruction* curInput = *input;
  if (curInput->isBoxed) {
    return;
  }
  BoxInstr* boxInstr = new BoxInstr(curInput);
  emit(boxInstr);
  curInput->usages.erase(instr);
  *input = boxInstr;
  boxInstr->usages.insert(instr);
}

void TypeAnalysis::visitPhiInstr(PhiInstr& instr) {
  VaivenStaticType commonType = instr.inputs[0]->type;
  bool numBoxed = instr.inputs[0]->isBoxed;
  for (vector<Instruction*>::iterator it = instr.inputs.begin() + 1;
      it != instr.inputs.end();
      ++it) {
    if ((*it)->isBoxed) {
      ++numBoxed;
    }

    if ((*it)->type != commonType) {
      commonType = VAIVEN_STATIC_TYPE_UNKNOWN;
      // we can break, we don't care about numBoxed in this case
      break;
    }
  }

  instr.type = commonType;
  if (commonType == VAIVEN_STATIC_TYPE_UNKNOWN) {
    for (vector<Instruction*>::iterator it = instr.inputs.begin();
        it != instr.inputs.end();
        ++it) {
      
      // visit it early, since phis can precede their inputs
      // make sure subsequent typechecks follow the phi
      // INVARIANT: Won't loop forever if it stops at PHIs
      if ((*it)->tag != INSTR_PHI) {
        Instruction* saveLastInstr = lastInstr;
        lastInstr = &instr;
        (*it)->accept(*this);
        lastInstr = saveLastInstr;
      }

      if (!(*it)->isBoxed || (*it)->tag == INSTR_PHI) {
        BoxInstr* boxed = new BoxInstr(*it);
        // For loops, the input can be after the phi. Therefore take care that we
        // box after the input, not before the phi.
        (*it)->append(boxed);

        (*it)->usages.erase(&instr);
        *it = boxed;
        boxed->usages.insert(&instr);
      }
    }
    instr.isBoxed = true;
  } else {
    if (numBoxed == instr.inputs.size()) {
      instr.isBoxed = true;
    } else if (numBoxed == 0) {
      instr.isBoxed = false;
    } else if (commonType == VAIVEN_STATIC_TYPE_DOUBLE) {
      // TODO check whether a boxed or unboxed PHI result is more useful. May be
      // more code to box the majority than unbox the minority, but if the
      // minority are boxed only to be unboxed, that may not be the best choice.
      if (numBoxed >= instr.inputs.size() / 2) {
        instr.isBoxed = true;
        // box the minority that aren't already
        for (vector<Instruction*>::iterator it = instr.inputs.begin();
            it != instr.inputs.end();
            ++it) {
          if (!(*it)->isBoxed) {
            BoxInstr* boxed = new BoxInstr(*it);
            // For loops, the input can be after the phi. Therefore take care that we
            // box after the input, not before the phi.
            (*it)->append(boxed);

            (*it)->usages.erase(&instr);
            *it = boxed;
            boxed->usages.insert(&instr);
          }
        }
      } else {
        instr.isBoxed = false;
        // unbox the minority that aren't already
        for (vector<Instruction*>::iterator it = instr.inputs.begin();
            it != instr.inputs.end();
            ++it) {
          if ((*it)->isBoxed) {
            UnboxInstr* unbox = new UnboxInstr(*it);
            // For loops, the input can be after the phi. Therefore take care that we
            // box after the input, not before the phi.
            (*it)->append(unbox);

            (*it)->usages.erase(&instr);
            *it = unbox;
            unbox->usages.insert(&instr);
          }
        }
      }
    } else {
      // For non-doubles, boxing is idempotent. Call them all unboxed.
      instr.isBoxed = false;
    }
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
    box(&*it, &instr);
  }
}

void TypeAnalysis::visitTypecheckInstr(TypecheckInstr& instr) {
}

void TypeAnalysis::visitBoxInstr(BoxInstr& instr) {
  instr.type = instr.inputs[0]->type;
}

void TypeAnalysis::visitUnboxInstr(UnboxInstr& instr) {
}

void TypeAnalysis::visitToDoubleInstr(ToDoubleInstr& instr) {
}

void TypeAnalysis::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
}

void TypeAnalysis::typecheckInput(Instruction& instr, VaivenStaticType expectedType, int input) {
  if (instr.inputs[input]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    // TODO cache this so we don't have to detect it with CSE
    TypecheckInstr* checkInstr = new TypecheckInstr(instr.inputs[input], expectedType);
    instr.inputs[input]->usages.insert(checkInstr);
    emit(checkInstr);
    // edge case: x + x. But this will be solved later in this function.
    instr.inputs[input]->usages.erase(&instr);
    checkInstr->usages.insert(&instr);
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

void TypeAnalysis::visitBinDoubleInstruction(Instruction& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    ToDoubleInstr* toDoubleInstr = new ToDoubleInstr(instr.inputs[0]);
    emit(toDoubleInstr);
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = toDoubleInstr;
    toDoubleInstr->usages.insert(&instr);
  } else if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT) {
    IntToDoubleInstr* toDoubleInstr = new IntToDoubleInstr(instr.inputs[0]);
    emit(toDoubleInstr);
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = toDoubleInstr;
    toDoubleInstr->usages.insert(&instr);
  } else if (instr.inputs[0]->type != VAIVEN_STATIC_TYPE_DOUBLE) {
    emit(new ErrInstr(EXPECTED_INT_OR_DOUBLE));
  }

  if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    ToDoubleInstr* toDoubleInstr = new ToDoubleInstr(instr.inputs[1]);
    emit(toDoubleInstr);
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs[1] = toDoubleInstr;
    toDoubleInstr->usages.insert(&instr);
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT) {
    IntToDoubleInstr* toDoubleInstr = new IntToDoubleInstr(instr.inputs[1]);
    emit(toDoubleInstr);
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs[1] = toDoubleInstr;
    toDoubleInstr->usages.insert(&instr);
  } else if (instr.inputs[1]->type != VAIVEN_STATIC_TYPE_DOUBLE) {
    emit(new ErrInstr(EXPECTED_INT_OR_DOUBLE));
  }

  if (instr.inputs[0]->isBoxed) {
    UnboxInstr* unboxInstr = new UnboxInstr(instr.inputs[0]);
    emit(unboxInstr);
    instr.inputs[0]->usages.erase(&instr);
    instr.inputs[0] = unboxInstr;
    unboxInstr->usages.insert(&instr);
  }

  if (instr.inputs[1]->isBoxed) {
    UnboxInstr* unboxInstr = new UnboxInstr(instr.inputs[1]);
    emit(unboxInstr);
    instr.inputs[1]->usages.erase(&instr);
    instr.inputs[1] = unboxInstr;
    unboxInstr->usages.insert(&instr);
  }
}

void TypeAnalysis::visitAddInstr(AddInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleAddInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    // any string means string addition. Next visit will insert typechecks
    instr.append(new StrAddInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntAddInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify strings, ints, doubles, or we couldn't
    // distinguish between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntAddInstr(IntAddInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleAddInstr(DoubleAddInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitStrAddInstr(StrAddInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_STRING, 0);
  typecheckInput(instr, VAIVEN_STATIC_TYPE_STRING, 1);
}

void TypeAnalysis::visitSubInstr(SubInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleSubInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntSubInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntSubInstr(IntSubInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleSubInstr(DoubleSubInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitMulInstr(MulInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleMulInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntMulInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntMulInstr(IntMulInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleMulInstr(DoubleMulInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitDivInstr(DivInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitNotInstr(NotInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_BOOL, 0);
}

void TypeAnalysis::visitCmpEqInstr(CmpEqInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleCmpEqInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_STRING
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_STRING) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntCmpEqInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleCmpIneqInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_STRING
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_STRING) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntCmpIneqInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitCmpGtInstr(CmpGtInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleCmpGtInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntCmpGtInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitCmpGteInstr(CmpGteInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleCmpGteInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntCmpGteInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitCmpLtInstr(CmpLtInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleCmpLtInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntCmpLtInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitCmpLteInstr(CmpLteInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_DOUBLE
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_DOUBLE) {
    // any double means double addition. Next visit should be on this node
    // where it should insert int to double conversions where necessary
    instr.append(new DoubleCmpLteInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else if ((instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT)
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // any int means int addition if there are no doubles. If there may be
    // doubles, we leave it as a runtime call for now, at least.
    instr.append(new IntCmpLteInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true;
  } else {
    // either we didn't identify ints, doubles, or we couldn't distinguish
    // between double and integer addition.
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
  visitBinIntInstruction(instr);
}

void TypeAnalysis::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
  visitBinDoubleInstruction(instr);
}

void TypeAnalysis::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_LIST) {
    instr.append(new ListAccessInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_OBJECT) {
    instr.append(new DynamicObjectAccessInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // always throws an error
    emit(new ErrInstr(EXPECTED_LIST_OR_OBJ));
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT) {
    instr.append(new ListAccessInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    instr.append(new DynamicObjectAccessInstr(instr.inputs[0], instr.inputs[1]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    emit(new ErrInstr(EXPECTED_STR_OR_INT));
  } else {
    // a true dynamic access. Must be boxed
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
  }
}

void TypeAnalysis::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_LIST) {
    instr.append(new ListStoreInstr(instr.inputs[0], instr.inputs[1], instr.inputs[2]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_OBJECT) {
    instr.append(new DynamicObjectStoreInstr(instr.inputs[0], instr.inputs[1], instr.inputs[2]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    // always throws an error
    emit(new ErrInstr(EXPECTED_LIST_OR_OBJ));
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT) {
    instr.append(new ListStoreInstr(instr.inputs[0], instr.inputs[1], instr.inputs[2]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    instr.append(new DynamicObjectStoreInstr(instr.inputs[0], instr.inputs[1], instr.inputs[2]));
    instr.replaceUsagesWith(instr.next);
    instr.safelyDeletable = true; // required for dead code elem since not pure
  } else if (instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
    emit(new ErrInstr(EXPECTED_STR_OR_INT));
  } else {
    // a true dynamic access. Must be boxed
    box(&instr.inputs[0], &instr);
    box(&instr.inputs[1], &instr);
    box(&instr.inputs[2], &instr);
  }
}

void TypeAnalysis::visitListAccessInstr(ListAccessInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_LIST, 0);
  typecheckInput(instr, VAIVEN_STATIC_TYPE_INT, 1);
}

void TypeAnalysis::visitListStoreInstr(ListStoreInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_LIST, 0);
  typecheckInput(instr, VAIVEN_STATIC_TYPE_INT, 1);
  box(&instr.inputs[2], &instr);
}

void TypeAnalysis::visitListInitInstr(ListInitInstr& instr) {
  for (vector<Instruction*>::iterator it = instr.inputs.begin();
      it != instr.inputs.end();
      ++it) {
    if (!(*it)->isBoxed) {
      box(&*it, &instr);
    }
  }
}

void TypeAnalysis::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_OBJECT, 0);
  typecheckInput(instr, VAIVEN_STATIC_TYPE_STRING, 1);
}

void TypeAnalysis::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_OBJECT, 0);
  typecheckInput(instr, VAIVEN_STATIC_TYPE_STRING, 1);
  box(&instr.inputs[2], &instr);
}

void TypeAnalysis::visitObjectAccessInstr(ObjectAccessInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_OBJECT, 0);
}

void TypeAnalysis::visitObjectStoreInstr(ObjectStoreInstr& instr) {
  typecheckInput(instr, VAIVEN_STATIC_TYPE_OBJECT, 0);
  box(&instr.inputs[1], &instr);
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
