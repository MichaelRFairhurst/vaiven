#include "emitter.h"
#include "../std.h"
#include "../heap.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void Emitter::visitPhiInstr(PhiInstr& instr) {
}

void Emitter::visitArgInstr(ArgInstr& instr) {
  args.push_back(&instr);
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
  } else if (instr.type == VAIVEN_STATIC_TYPE_STRING) {
    cc.mov(checkArg, MAX_PTR);
    cc.cmp(instr.out, checkArg);
    cc.jg(deoptimizeLabel);
    cc.mov(checkArg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkArg.r32(), GCABLE_TYPE_STRING);
    cc.jne(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_LIST) {
    cc.mov(checkArg, MAX_PTR);
    cc.cmp(instr.out, checkArg);
    cc.jg(deoptimizeLabel);
    cc.mov(checkArg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkArg.r32(), GCABLE_TYPE_LIST);
    cc.jne(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_OBJECT) {
    cc.mov(checkArg, MAX_PTR);
    cc.cmp(instr.out, checkArg);
    cc.jg(deoptimizeLabel);
    cc.mov(checkArg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkArg.r32(), GCABLE_TYPE_OBJECT);
    cc.jne(deoptimizeLabel);
  }
  afterGuardsLabel = cc.newLabel();
  cc.bind(afterGuardsLabel);
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
  if (!instr.func.isNative
      && instr.func.ast->name == funcName
      && instr.next != NULL
      && instr.next->tag == INSTR_RET
      && instr.next->inputs[0] == &instr) {
    // crappy tail recursion algorithm
    vector<ArgInstr*>::iterator argsIt = args.begin();
    vector<Instruction*>::iterator inputsIt = instr.inputs.begin();
    bool matchesGuards = true;

    while (argsIt != args.end()) {
      if (inputsIt != instr.inputs.end()) {
        if ((*argsIt)->type != (*inputsIt)->type) {
          matchesGuards = false;
          break;
        }
        ++inputsIt;
      } else {
        if ((*argsIt)->type != VAIVEN_STATIC_TYPE_VOID) {
          matchesGuards = false;
          break;
        }
      }
      ++argsIt;
    }

    if (matchesGuards) {
      argsIt = args.begin();
      inputsIt = instr.inputs.begin();

      while (argsIt != args.end()) {
        if (inputsIt != instr.inputs.end()) {
          if ((*argsIt)->out != (*inputsIt)->out) {
            cc.mov((*argsIt)->out, (*inputsIt)->out);
          }
          ++inputsIt;
        } else {
          if ((*argsIt)->type != VAIVEN_STATIC_TYPE_VOID) {
            cc.mov((*argsIt)->out, VOID);
          }
        }
        ++argsIt;
      }

      cc.jmp(afterGuardsLabel);
      return;
    }
  }

  int argc = instr.func.argc;
  int paramc = instr.inputs.size();
  vector<X86Gp> voidRegs;

  for (int i = paramc; i < argc; ++i) {
    X86Gp void_ = cc.newUInt64();
    cc.mov(void_, VOID);
    voidRegs.push_back(void_);
  }
 
  uint8_t sigArgs[argc];
  for (int i = 0; i < argc; ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, argc);

  CCFuncCall* call;
  if (instr.funcName == funcName) {
    call = cc.call(funcLabel, sig);
  } else if (instr.func.isNative) {
    call = cc.call((uint64_t) instr.func.fptr, sig);
  } else {
    X86Gp lookup = cc.newUInt64();
    cc.mov(lookup, (uint64_t) &instr.func.fptr);
    call = cc.call(x86::ptr(lookup), sig);
  }

  for (int i = 0; i < paramc; ++i) {
    call->setArg(i, instr.inputs[i]->out);
  }

  for (int i = paramc; i < argc; ++i) {
    call->setArg(i, voidRegs[i - paramc]);
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
  } else if (instr.type == VAIVEN_STATIC_TYPE_STRING) {
    cc.mov(checkReg, MAX_PTR);
    cc.cmp(instr.out, checkReg);
    cc.jg(deoptimizeLabel);
    cc.mov(checkReg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkReg.r32(), GCABLE_TYPE_STRING);
    cc.jne(error.stringTypeErrorLabel);
    error.hasStringTypeError = true;
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
  CCFuncCall* call = cc.call((uint64_t) vaiven::add, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitStrAddInstr(StrAddInstr& instr) {
  CCFuncCall* call = cc.call((uint64_t) vaiven::addStrUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntAddInstr(IntAddInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out, instr.inputs[0]->out);
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
  cc.setz(instr.out.r8());
}

void Emitter::visitCmpEqInstr(CmpEqInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
    && instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    CCFuncCall* call = cc.call((uint64_t) vaiven::cmpStrUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
    call->setArg(0, instr.inputs[0]->out);
    call->setArg(1, instr.inputs[1]->out);
    call->setRet(0, instr.out);
    return;
  } else if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
    || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING
    || instr.inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN
    || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    CCFuncCall* call = cc.call((uint64_t) vaiven::cmpUnboxed, FuncSignature2<uint64_t, uint64_t, uint64_t>());
    call->setArg(0, instr.inputs[0]->out);
    call->setArg(1, instr.inputs[1]->out);
    call->setRet(0, instr.out);
    return;
  }

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doCmpEqInstr(instr);
  cc.setz(instr.out);
}

void Emitter::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
    && instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    CCFuncCall* call = cc.call((uint64_t) vaiven::inverseCmpStrUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
    call->setArg(0, instr.inputs[0]->out);
    call->setArg(1, instr.inputs[1]->out);
    call->setRet(0, instr.out);
    return;
  } else if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
    || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING
    || instr.inputs[0]->type == VAIVEN_STATIC_TYPE_UNKNOWN
    || instr.inputs[1]->type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    CCFuncCall* call = cc.call((uint64_t) vaiven::inverseCmpUnboxed, FuncSignature2<uint64_t, uint64_t, uint64_t>());
    call->setArg(0, instr.inputs[0]->out);
    call->setArg(1, instr.inputs[1]->out);
    call->setRet(0, instr.out);
    return;
  }

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
  } else if (!instr.inputs[0]->isBoxed
      && instr.inputs[0]->type & (VAIVEN_STATIC_TYPE_INT | VAIVEN_STATIC_TYPE_BOOL)) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  } else {
    cc.cmp(instr.inputs[0]->out, instr.inputs[1]->out);
  }
}

void Emitter::doCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constI32Rhs);
  } else if (!instr.inputs[0]->isBoxed
      && instr.inputs[0]->type & (VAIVEN_STATIC_TYPE_INT | VAIVEN_STATIC_TYPE_BOOL)) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
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

void Emitter::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
  CCFuncCall* call = cc.call((uint64_t) vaiven::get, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
  CCFuncCall* call = cc.call((uint64_t) vaiven::set, FuncSignature3<uint64_t, uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setArg(2, instr.inputs[2]->out);
}

void Emitter::visitListAccessInstr(ListAccessInstr& instr) {
  CCFuncCall* call = cc.call((uint64_t) vaiven::listAccessUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitListStoreInstr(ListStoreInstr& instr) {
  CCFuncCall* call = cc.call((uint64_t) vaiven::listStoreUnchecked, FuncSignature3<uint64_t, uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setArg(2, instr.inputs[2]->out);
}

void Emitter::visitListInitInstr(ListInitInstr& instr) {
  X86Gp size = cc.newUInt64();
  cc.mov(size, instr.inputs.size());
  CCFuncCall* alloc = cc.call((uint64_t) newListWithSize, FuncSignature1<uint64_t, uint64_t>());
  alloc->setArg(0, size);
  alloc->setRet(0, instr.out);

  CCFuncCall* getPtr = cc.call((uint64_t) getListContainerUnchecked, FuncSignature1<uint64_t, uint64_t>());
  getPtr->setArg(0, instr.out);
  X86Gp ptr = cc.newUInt64();
  getPtr->setRet(0, ptr);

  for (vector<Instruction*>::iterator it = instr.inputs.begin();
      it != instr.inputs.end();
      ++it) {
    if (it != instr.inputs.begin()) {
      cc.add(ptr, sizeof(Value));
    }

    cc.mov(x86::ptr(ptr), (*it)->out);
  }
}

void Emitter::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  X86Gp str;
  if (instr.inputs[1]->usages.size() == 1) {
    str = instr.inputs[1]->out;
  } else {
    str = cc.newUInt64();
    cc.mov(str, instr.inputs[1]->out);
  }
  cc.add(str, 8); // string member offset
  CCFuncCall* call = cc.call((uint64_t) vaiven::objectAccessUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, str);
  call->setRet(0, instr.out);
}

void Emitter::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
  X86Gp str;
  if (instr.inputs[1]->usages.size() == 1) {
    str = instr.inputs[1]->out;
  } else {
    str = cc.newUInt64();
    cc.mov(str, instr.inputs[1]->out);
  }
  cc.add(str, 8); // string member offset
  CCFuncCall* call = cc.call((uint64_t) vaiven::objectStoreUnchecked, FuncSignature3<uint64_t, uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, str);
  call->setArg(2, instr.inputs[2]->out);
}

void Emitter::visitObjectAccessInstr(ObjectAccessInstr& instr) {
  X86Gp str = cc.newUInt64();
  cc.mov(str, (uint64_t) instr.property);
  CCFuncCall* call = cc.call((uint64_t) vaiven::objectAccessUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, str);
  call->setRet(0, instr.out);
}

void Emitter::visitObjectStoreInstr(ObjectStoreInstr& instr) {
  X86Gp str = cc.newUInt64();
  cc.mov(str, (uint64_t) instr.property);
  CCFuncCall* call = cc.call((uint64_t) vaiven::objectStoreUnchecked, FuncSignature3<uint64_t, uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, str);
  call->setArg(2, instr.inputs[1]->out);
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
      cc.test(exit.condition->inputs[0]->out.r32(), exit.condition->inputs[0]->out.r32());
      cc.jz(exit.toGoTo->label);
      break;
    default:
      exit.condition->accept(*this);
      cc.test(exit.condition->out.r32(), exit.condition->out.r32());
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
    for (std::set<Instruction*>::iterator it = next->usages.begin(); it != next->usages.end(); ++it) {
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
