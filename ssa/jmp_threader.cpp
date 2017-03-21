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

void JmpThreader::visitAddInstr(AddInstr& instr) {
}

void JmpThreader::visitIntAddInstr(IntAddInstr& instr) {
}

void JmpThreader::visitStrAddInstr(StrAddInstr& instr) {
}

void JmpThreader::visitSubInstr(SubInstr& instr) {
}

void JmpThreader::visitMulInstr(MulInstr& instr) {
}

void JmpThreader::visitDivInstr(DivInstr& instr) {
}

void JmpThreader::visitNotInstr(NotInstr& instr) {
}

void JmpThreader::visitCmpEqInstr(CmpEqInstr& instr) {
}

void JmpThreader::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void JmpThreader::visitCmpGtInstr(CmpGtInstr& instr) {
}

void JmpThreader::visitCmpGteInstr(CmpGteInstr& instr) {
}

void JmpThreader::visitCmpLtInstr(CmpLtInstr& instr) {
}

void JmpThreader::visitCmpLteInstr(CmpLteInstr& instr) {
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

void JmpThreader::visitErrInstr(ErrInstr& instr) {
}

void JmpThreader::visitRetInstr(RetInstr& instr) {
}

void JmpThreader::visitJmpCcInstr(JmpCcInstr& instr) {
}

void JmpThreader::visitUnconditionalBlockExit(UnconditionalBlockExit& exit) {
  while (exit.toGoTo->head == NULL) {
    //if (exit.toGoTo->exits.size() == 0 && exit.toGoTo->next != NULL) {
    //  exit.toGoTo = &*exit.toGoTo->next;
    //} else
    if (exit.toGoTo->exits.size() == 1
          && exit.toGoTo->exits[0]->tag == BLOCK_EXIT_UNCONDITIONAL) {
      exit.toGoTo = exit.toGoTo->exits[0]->toGoTo;
    } else {
      break;
    }
    // TODO jump thread conditional exits
  }
}

void JmpThreader::visitConditionalBlockExit(ConditionalBlockExit& exit) {
  while (exit.toGoTo->head == NULL) {
    //if (exit.toGoTo->exits.size() == 0 && exit.toGoTo->next != NULL) {
    //  exit.toGoTo = &*exit.toGoTo->next;
    //} else
    if (exit.toGoTo->exits.size() == 1
          && exit.toGoTo->exits[0]->tag == BLOCK_EXIT_UNCONDITIONAL) {
      exit.toGoTo = exit.toGoTo->exits[0]->toGoTo;
    } else {
      break;
    }
    // TODO jump thread conditional exits
  }

  if (exit.condition->tag != INSTR_JMPCC) {
    return;
  }

  JmpCcInstr& jmpInstr = static_cast<JmpCcInstr&>(*exit.condition);
  Instruction* condition = jmpInstr.inputs[0];
  Instruction* replacementInstr;
  switch(condition->tag) {
    case INSTR_CMPEQ:
    {
      CmpEqInstr* conditionEq = static_cast<CmpEqInstr*>(condition);
      if (conditionEq->hasConstRhs) {
        replacementInstr = new CmpEqInstr(condition->inputs[0], conditionEq->constI32Rhs);
      } else {
        if (condition->inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
            || condition->inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN
            || condition->inputs[1]->type == VAIVEN_STATIC_TYPE_STRING
            || condition->inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
          // will be compiled as a call, test rax so we can do strcmp
          // TODO check where exact type is unknown but can't be a string
          return;
        }
        replacementInstr = new CmpEqInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_CMPINEQ:
    {
      CmpIneqInstr* conditionIneq = static_cast<CmpIneqInstr*>(condition);
      if (conditionIneq->hasConstRhs) {
        replacementInstr = new CmpIneqInstr(condition->inputs[0], conditionIneq->constI32Rhs);
      } else {
        if (condition->inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
            || condition->inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN
            || condition->inputs[1]->type == VAIVEN_STATIC_TYPE_STRING
            || condition->inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
          // will be compiled as a call, test rax so we can do strcmp
          // TODO check where exact type is unknown but can't be a string
          return;
        }
        replacementInstr = new CmpIneqInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_CMPGT:
    {
      CmpGtInstr* conditionGt = static_cast<CmpGtInstr*>(condition);
      if (conditionGt->hasConstRhs) {
        replacementInstr = new CmpGtInstr(condition->inputs[0], conditionGt->constRhs);
      } else {
        replacementInstr = new CmpGtInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_CMPGTE:
    {
      CmpGteInstr* conditionGte = static_cast<CmpGteInstr*>(condition);
      if (conditionGte->hasConstRhs) {
        replacementInstr = new CmpGteInstr(condition->inputs[0], conditionGte->constRhs);
      } else {
        replacementInstr = new CmpGteInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_CMPLT:
    {
      CmpLtInstr* conditionLt = static_cast<CmpLtInstr*>(condition);
      if (conditionLt->hasConstRhs) {
        replacementInstr = new CmpLtInstr(condition->inputs[0], conditionLt->constRhs);
      } else {
        replacementInstr = new CmpLtInstr(condition->inputs[0], condition->inputs[1]);
      }
      break;
    }
    case INSTR_CMPLTE:
    {
      CmpLteInstr* conditionLte = static_cast<CmpLteInstr*>(condition);
      if (conditionLte->hasConstRhs) {
        replacementInstr = new CmpLteInstr(condition->inputs[0], conditionLte->constRhs);
      } else {
        replacementInstr = new CmpLteInstr(condition->inputs[0], condition->inputs[1]);
      }
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
