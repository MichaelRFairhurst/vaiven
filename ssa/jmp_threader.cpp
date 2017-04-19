#include "jmp_threader.h"

using namespace vaiven::ssa;

void JmpThreader::visitPhiInstr(PhiInstr& instr) {
}

void JmpThreader::visitArgInstr(ArgInstr& instr) {
}

void JmpThreader::visitConstantInstr(ConstantInstr& instr) {
}

void JmpThreader::visitCallInstr(CallInstr& instr) {
}

void JmpThreader::visitTypecheckInstr(TypecheckInstr& instr) {
}

void JmpThreader::visitBoxInstr(BoxInstr& instr) {
}

void JmpThreader::visitUnboxInstr(UnboxInstr& instr) {
}

void JmpThreader::visitToDoubleInstr(ToDoubleInstr& instr) {
}

void JmpThreader::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
}

void JmpThreader::visitAddInstr(AddInstr& instr) {
}

void JmpThreader::visitIntAddInstr(IntAddInstr& instr) {
}

void JmpThreader::visitDoubleAddInstr(DoubleAddInstr& instr) {
}

void JmpThreader::visitStrAddInstr(StrAddInstr& instr) {
}

void JmpThreader::visitSubInstr(SubInstr& instr) {
}

void JmpThreader::visitIntSubInstr(IntSubInstr& instr) {
}

void JmpThreader::visitDoubleSubInstr(DoubleSubInstr& instr) {
}

void JmpThreader::visitMulInstr(MulInstr& instr) {
}

void JmpThreader::visitIntMulInstr(IntMulInstr& instr) {
}

void JmpThreader::visitDoubleMulInstr(DoubleMulInstr& instr) {
}

void JmpThreader::visitDivInstr(DivInstr& instr) {
}

void JmpThreader::visitNotInstr(NotInstr& instr) {
}

void JmpThreader::visitCmpEqInstr(CmpEqInstr& instr) {
}

void JmpThreader::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
}

void JmpThreader::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
}

void JmpThreader::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void JmpThreader::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
}

void JmpThreader::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
}

void JmpThreader::visitCmpGtInstr(CmpGtInstr& instr) {
}

void JmpThreader::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
}

void JmpThreader::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
}

void JmpThreader::visitCmpGteInstr(CmpGteInstr& instr) {
}

void JmpThreader::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
}

void JmpThreader::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
}

void JmpThreader::visitCmpLtInstr(CmpLtInstr& instr) {
}

void JmpThreader::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
}

void JmpThreader::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
}

void JmpThreader::visitCmpLteInstr(CmpLteInstr& instr) {
}

void JmpThreader::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
}

void JmpThreader::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
}

void JmpThreader::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
}

void JmpThreader::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
}

void JmpThreader::visitListAccessInstr(ListAccessInstr& instr) {
}

void JmpThreader::visitListStoreInstr(ListStoreInstr& instr) {
}

void JmpThreader::visitListInitInstr(ListInitInstr& instr) {
}

void JmpThreader::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
}

void JmpThreader::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
}

void JmpThreader::visitObjectAccessInstr(ObjectAccessInstr& instr) {
}

void JmpThreader::visitObjectStoreInstr(ObjectStoreInstr& instr) {
}

void JmpThreader::visitErrInstr(ErrInstr& instr) {
}

void JmpThreader::visitRetInstr(RetInstr& instr) {
}

void JmpThreader::visitJmpCcInstr(JmpCcInstr& instr) {
}

void JmpThreader::visitUnconditionalBlockExit(UnconditionalBlockExit& exit) {
  set<Block*> infiniteLoopDetect;
  // TODO jump thread conditional exits
  while (exit.toGoTo->head.get() == NULL
      && exit.toGoTo->exits.size() == 1
      && exit.toGoTo->exits[0]->tag == BLOCK_EXIT_UNCONDITIONAL
      && infiniteLoopDetect.find(exit.toGoTo) == infiniteLoopDetect.end()) {
    infiniteLoopDetect.insert(exit.toGoTo);
    // INVARIANT: We don't need to check if curBlock has multiple pointers
    // into this block before we unlink them, because all exits to this dead
    // block will be threaded, and its safe to erase mulitple times, insert
    // multiple times.
    exit.toGoTo->immPredecessors.erase(curBlock);
    exit.toGoTo = exit.toGoTo->exits[0]->toGoTo;
    exit.toGoTo->immPredecessors.insert(curBlock);

    // we don't actually do this yet though because no optimizations depend
    // on it
    requiresRebuildDominators = true;
  }
}

void JmpThreader::visitConditionalBlockExit(ConditionalBlockExit& exit) {
  set<Block*> infiniteLoopDetect;
  // TODO jump thread conditional exits
  while (exit.toGoTo->head.get() == NULL
      && exit.toGoTo->exits.size() == 1
      && exit.toGoTo->exits[0]->tag == BLOCK_EXIT_UNCONDITIONAL
      && infiniteLoopDetect.find(exit.toGoTo) == infiniteLoopDetect.end()) {
    infiniteLoopDetect.insert(exit.toGoTo);
    // INVARIANT: We don't need to check if curBlock has multiple pointers
    // into this block before we unlink them, because all exits to this dead
    // block will be threaded, and its safe to erase mulitple times, insert
    // multiple times.
    exit.toGoTo->immPredecessors.erase(curBlock);
    exit.toGoTo = exit.toGoTo->exits[0]->toGoTo;
    exit.toGoTo->immPredecessors.insert(curBlock);

    // we don't actually do this yet though because no optimizations depend
    // on it
    requiresRebuildDominators = true;
  }

  if (exit.condition->tag != INSTR_JMPCC) {
    return;
  }

  JmpCcInstr& jmpInstr = static_cast<JmpCcInstr&>(*exit.condition);
  Instruction* condition = jmpInstr.inputs[0];
  Instruction* replacementInstr;
  switch(condition->tag) {
    case INSTR_INT_CMPEQ:
    {
      IntCmpEqInstr* conditionEq = static_cast<IntCmpEqInstr*>(condition);
      if (conditionEq->hasConstRhs) {
        replacementInstr = new IntCmpEqInstr(condition->inputs[0], conditionEq->constI32Rhs);
      } else {
        replacementInstr = new IntCmpEqInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_INT_CMPINEQ:
    {
      IntCmpIneqInstr* conditionIneq = static_cast<IntCmpIneqInstr*>(condition);
      if (conditionIneq->hasConstRhs) {
        replacementInstr = new IntCmpIneqInstr(condition->inputs[0], conditionIneq->constI32Rhs);
      } else {
        replacementInstr = new IntCmpIneqInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_INT_CMPGT:
    {
      IntCmpGtInstr* conditionGt = static_cast<IntCmpGtInstr*>(condition);
      if (conditionGt->hasConstRhs) {
        replacementInstr = new IntCmpGtInstr(condition->inputs[0], conditionGt->constRhs);
      } else {
        replacementInstr = new IntCmpGtInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_INT_CMPGTE:
    {
      IntCmpGteInstr* conditionGte = static_cast<IntCmpGteInstr*>(condition);
      if (conditionGte->hasConstRhs) {
        replacementInstr = new IntCmpGteInstr(condition->inputs[0], conditionGte->constRhs);
      } else {
        replacementInstr = new IntCmpGteInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_INT_CMPLT:
    {
      IntCmpLtInstr* conditionLt = static_cast<IntCmpLtInstr*>(condition);
      if (conditionLt->hasConstRhs) {
        replacementInstr = new IntCmpLtInstr(condition->inputs[0], conditionLt->constRhs);
      } else {
        replacementInstr = new IntCmpLtInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_INT_CMPLTE:
    {
      IntCmpLteInstr* conditionLte = static_cast<IntCmpLteInstr*>(condition);
      if (conditionLte->hasConstRhs) {
        replacementInstr = new IntCmpLteInstr(condition->inputs[0], conditionLte->constRhs);
      } else {
        replacementInstr = new IntCmpLteInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_DBL_CMPEQ:
    {
      DoubleCmpEqInstr* conditionEq = static_cast<DoubleCmpEqInstr*>(condition);
      replacementInstr = new DoubleCmpEqInstr(condition->inputs[0], condition->inputs[1]);
      break;
    }
    case INSTR_DBL_CMPINEQ:
    {
      DoubleCmpIneqInstr* conditionIneq = static_cast<DoubleCmpIneqInstr*>(condition);
      replacementInstr = new DoubleCmpIneqInstr(condition->inputs[0], condition->inputs[1]);
      break;
    }
    case INSTR_DBL_CMPGT:
    {
      DoubleCmpGtInstr* conditionGt = static_cast<DoubleCmpGtInstr*>(condition);
      replacementInstr = new DoubleCmpGtInstr(condition->inputs[0], condition->inputs[1]);
      break;
    }
    case INSTR_DBL_CMPGTE:
    {
      DoubleCmpGteInstr* conditionGte = static_cast<DoubleCmpGteInstr*>(condition);
      replacementInstr = new DoubleCmpGteInstr(condition->inputs[0], condition->inputs[1]);
      break;
    }
    case INSTR_DBL_CMPLT:
    {
      DoubleCmpLtInstr* conditionLt = static_cast<DoubleCmpLtInstr*>(condition);
      replacementInstr = new DoubleCmpLtInstr(condition->inputs[0], condition->inputs[1]);
      break;
    }
    case INSTR_DBL_CMPLTE:
    {
      DoubleCmpLteInstr* conditionLte = static_cast<DoubleCmpLteInstr*>(condition);
      replacementInstr = new DoubleCmpLteInstr(condition->inputs[0], condition->inputs[1]);
      break;
    }
    case INSTR_NOT:
    {
      replacementInstr = new NotInstr(condition->inputs[0]);
      break;
    }
    default:
      return;
  }

  condition->usages.erase(&jmpInstr);
  exit.condition.reset(replacementInstr);
}
