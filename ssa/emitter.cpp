#include "emitter.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void Emitter::visitPhiInstr(PhiInstr& instr) {
}

void Emitter::visitArgInstr(ArgInstr& instr) {
  X86Gp checkArg = cc.newInt64();
  if (instr.type == VAIVEN_STATIC_TYPE_INT) {
    cc.mov(checkArg, instr.out);
    cc.shr(checkArg, VALUE_TAG_SHIFT);
    cc.cmp(checkArg, INT_TAG_SHIFTED);
    cc.jne(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_BOOL) {
    cc.mov(checkArg, instr.out);
    cc.shr(checkArg, VALUE_TAG_SHIFT);
    cc.cmp(checkArg, BOOL_TAG_SHIFTED);
    cc.jne(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_VOID) {
    cc.mov(checkArg, VOID);
    cc.cmp(instr.out, checkArg);
    cc.jne(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_POINTER) {
    cc.mov(checkArg, MAX_PTR);
    cc.cmp(instr.out, checkArg);
    cc.jg(deoptimizeLabel);
  }
}

void Emitter::visitConstantInstr(ConstantInstr& instr) {
  if (instr.isBoxed || instr.val.isPtr()) {
    cc.mov(instr.out, instr.val.getRaw());
  } else if (instr.val.isInt() || instr.val.isBool() || instr.val.isVoid()) {
    cc.mov(instr.out.r32(), instr.val.getInt());
  } else {
    cc.mov(instr.out, instr.val.getDouble());
  }
}

void Emitter::visitCallInstr(CallInstr& instr) {
  // TODO recursion
  if (funcs.funcs.find(instr.funcName) == funcs.funcs.end()) {
    error.noFuncError();
    return;
  }
  // TODO check arg counts

  uint8_t sigArgs[instr.inputs.size()];
  for (int i = 0; i < instr.inputs.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, instr.inputs.size());

  CCFuncCall* call;
  if (instr.funcName == funcName) {
    call = cc.call(funcLabel, sig);
  } else {
    X86Gp lookup = cc.newUInt64();
    cc.mov(lookup, (uint64_t) &funcs.funcs[instr.funcName]->fptr);
    call = cc.call(x86::ptr(lookup), sig);
  }

  for (int i = 0; i < instr.inputs.size(); ++i) {
    call->setArg(i, instr.inputs[i]->out);
  }

  call->setRet(0, instr.out);
}

void Emitter::visitTypecheckInstr(TypecheckInstr& instr) {
  X86Gp checkReg = cc.newUInt64();
  if (instr.type == VAIVEN_STATIC_TYPE_INT) {
    cc.mov(checkReg, instr.out);
    cc.shr(checkReg, VALUE_TAG_SHIFT);
    cc.cmp(checkReg, INT_TAG_SHIFTED);
    cc.jne(error.intTypeErrorLabel);
    error.hasIntTypeError = true;
  } else if (instr.type == VAIVEN_STATIC_TYPE_BOOL) {
    cc.mov(checkReg, instr.out);
    cc.shr(checkReg, VALUE_TAG_SHIFT);
    cc.cmp(checkReg, BOOL_TAG_SHIFTED);
    cc.jne(error.boolTypeErrorLabel);
    error.hasBoolTypeError = true;
  }
}

void Emitter::visitBoxInstr(BoxInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out, instr.inputs[0]->out);
  }

  X86Gp toOrReg = cc.newUInt64();
  if (instr.type == VAIVEN_STATIC_TYPE_INT) {
    cc.mov(toOrReg, INT_TAG);
    cc.or_(instr.out, toOrReg);
  } else if (instr.type == VAIVEN_STATIC_TYPE_BOOL) {
    cc.mov(toOrReg, BOOL_TAG);
    cc.or_(instr.out, toOrReg);
  }
}

void Emitter::visitAddInstr(AddInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out.r32(), instr.inputs[0]->out.r32());
  }

  if (instr.hasConstRhs) {
    cc.add(instr.out.r32(), instr.constRhs);
  } else {
    cc.add(instr.out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::visitSubInstr(SubInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out.r32(), instr.inputs[0]->out.r32());
  }

  if (instr.hasConstLhs) {
    cc.neg(instr.out.r32());
    cc.add(instr.out.r32(), instr.constLhs);
  } else if (instr.isInverse) {
    cc.neg(instr.out.r32());
    cc.add(instr.out.r32(), instr.inputs[1]->out.r32());
  } else {
    cc.sub(instr.out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::visitMulInstr(MulInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out.r32(), instr.inputs[0]->out.r32());
  }


  if (instr.hasConstRhs) {
    cc.imul(instr.out.r32(), instr.constRhs);
  } else {
    cc.imul(instr.out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::visitDivInstr(DivInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out.r32(), instr.inputs[0]->out.r32());
  }

  X86Gp dummy = cc.newUInt64();
  cc.xor_(dummy, dummy);
  cc.idiv(dummy.r32(), instr.out.r32(), instr.inputs[1]->out.r32());
}

void Emitter::visitNotInstr(NotInstr& instr) {
  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  cc.test(instr.inputs[0]->out.r32(), instr.inputs[0]->out.r32());
  cc.setz(instr.out);
}

void Emitter::visitCmpEqInstr(CmpEqInstr& instr) {
  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doCmpEqInstr(instr);
  cc.setz(instr.out);
}

void Emitter::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doCmpIneqInstr(instr);
  cc.setnz(instr.out);
}

void Emitter::visitCmpGtInstr(CmpGtInstr& instr) {
  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doCmpGtInstr(instr);
  cc.setg(instr.out);
}

void Emitter::visitCmpGteInstr(CmpGteInstr& instr) {
  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doCmpGteInstr(instr);
  cc.setge(instr.out);
}

void Emitter::visitCmpLtInstr(CmpLtInstr& instr) {
  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doCmpLtInstr(instr);
  cc.setl(instr.out);
}

void Emitter::visitCmpLteInstr(CmpLteInstr& instr) {
  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doCmpLteInstr(instr);
  cc.setle(instr.out);
}

void Emitter::doCmpEqInstr(CmpEqInstr& instr) {
  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constI32Rhs);
  } else {
    cc.cmp(instr.inputs[0]->out, instr.inputs[1]->out);
  }
}

void Emitter::doCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constI32Rhs);
  } else {
    cc.cmp(instr.inputs[0]->out, instr.inputs[1]->out);
  }
}

void Emitter::doCmpGtInstr(CmpGtInstr& instr) {
  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doCmpGteInstr(CmpGteInstr& instr) {
  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doCmpLtInstr(CmpLtInstr& instr) {
  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doCmpLteInstr(CmpLteInstr& instr) {
  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::visitErrInstr(ErrInstr& instr) {
  if (instr.error == EXPECTED_INT) {
    error.intTypeError();
  } else if (instr.error == EXPECTED_BOOL) {
    error.intTypeError();
  } else if (instr.error == NO_SUCH_FUNCTION) {
    error.noFuncError();
  } else if (instr.error == NO_SUCH_VAR) {
    error.noVarError();
  } else if (instr.error == DUPLICATE_VAR) {
    error.dupVarError();
  }
}

void Emitter::visitRetInstr(RetInstr& instr) {
  cc.ret(instr.inputs[0]->out);
}

void Emitter::visitUnconditionalBlockExit(UnconditionalBlockExit& exit) {
  if (exit.toGoTo != &*curBlock->next) {
    cc.jmp(exit.toGoTo->label);
  }
}

void Emitter::visitConditionalBlockExit(ConditionalBlockExit& exit) {
  switch(exit.condition->tag) {
    case INSTR_CMPEQ:
      doCmpEqInstr(static_cast<CmpEqInstr&>(*exit.condition));
      cc.je(exit.toGoTo->label);
      break;
    case INSTR_CMPINEQ:
      doCmpIneqInstr(static_cast<CmpIneqInstr&>(*exit.condition));
      cc.jne(exit.toGoTo->label);
      break;
    case INSTR_CMPGT:
      doCmpGtInstr(static_cast<CmpGtInstr&>(*exit.condition));
      cc.jg(exit.toGoTo->label);
      break;
    case INSTR_CMPGTE:
      doCmpGteInstr(static_cast<CmpGteInstr&>(*exit.condition));
      cc.jge(exit.toGoTo->label);
      break;
    case INSTR_CMPLT:
      doCmpLtInstr(static_cast<CmpLtInstr&>(*exit.condition));
      cc.jl(exit.toGoTo->label);
      break;
    case INSTR_CMPLTE:
      doCmpLteInstr(static_cast<CmpLteInstr&>(*exit.condition));
      cc.jle(exit.toGoTo->label);
      break;
    case INSTR_NOT:
      cc.test(exit.condition->inputs[0]->out, exit.condition->inputs[0]->out);
      cc.jz(exit.toGoTo->label);
      break;
    default:
      exit.condition->accept(*this);
      cc.test(exit.condition->out, exit.condition->out);
      cc.jnz(exit.toGoTo->label);
  }
}

void Emitter::visitBlock(Block& block) {
  cc.bind(block.label);
  curBlock = &block;
  lastInstr = NULL;
  Instruction* next = block.head.get();
  while (next != NULL) {
    next->accept(*this);
    // handle PHIs
    for (set<Instruction*>::iterator it = next->usages.begin(); it != next->usages.end(); ++it) {
      if ((*it)->tag == INSTR_PHI && (*it)->out != next->out) {
        cc.mov((*it)->out, next->out);
      }
    }
    lastInstr = next;
    next = next->next;
  }

  for (vector<unique_ptr<BlockExit>>::iterator it = block.exits.begin();
      it != block.exits.end();
      ++it) {
    (*it)->accept(*this);
  }

  if (block.next != NULL) {
    block.next->accept(*this);
  }
}

void Emitter::visitJmpCcInstr(JmpCcInstr& instr) {
}
