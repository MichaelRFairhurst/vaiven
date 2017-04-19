#include "emitter.h"
#include "../std.h"
#include "../heap.h"
#include <cassert>

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void Emitter::visitPhiInstr(PhiInstr& instr) {
}

void Emitter::visitArgInstr(ArgInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

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
  } else if (instr.type == VAIVEN_STATIC_TYPE_DOUBLE) {
    cc.mov(checkArg, MIN_DBL);
    cc.cmp(instr.out, checkArg);
    cc.jb(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_STRING) {
    cc.mov(checkArg, MAX_PTR);
    cc.cmp(instr.out, checkArg);
    cc.ja(deoptimizeLabel);
    cc.mov(checkArg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkArg.r32(), GCABLE_TYPE_STRING);
    cc.jne(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_LIST) {
    cc.mov(checkArg, MAX_PTR);
    cc.cmp(instr.out, checkArg);
    cc.ja(deoptimizeLabel);
    cc.mov(checkArg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkArg.r32(), GCABLE_TYPE_LIST);
    cc.jne(deoptimizeLabel);
  } else if (instr.type == VAIVEN_STATIC_TYPE_OBJECT) {
    cc.mov(checkArg, MAX_PTR);
    cc.cmp(instr.out, checkArg);
    cc.ja(deoptimizeLabel);
    cc.mov(checkArg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkArg.r32(), GCABLE_TYPE_OBJECT);
    cc.jne(deoptimizeLabel);
  }
  afterGuardsLabel = cc.newLabel();
  cc.bind(afterGuardsLabel);
}

void Emitter::visitConstantInstr(ConstantInstr& instr) {
  if (instr.isBoxed || instr.val.isPtr()) {
    assert(cc.isVirtRegValid(instr.out));
    cc.mov(instr.out, instr.val.getRaw());
  } else if (instr.val.isInt() || instr.val.isBool() || instr.val.isVoid()) {
    assert(cc.isVirtRegValid(instr.out));
    cc.mov(instr.out.r32(), instr.val.getInt());
  } else {
    X86Mem mem = cc.newStack(8, 8);
    X86Gp temp = cc.newUInt64();
    cc.mov(temp, ~instr.val.getRaw());
    cc.mov(mem, temp);
    assert(cc.isVirtRegValid(instr.xmmout));
    cc.movsd(instr.xmmout, mem);
  }
}

void Emitter::visitCallInstr(CallInstr& instr) {
  if (!instr.func.isNative
      && instr.func.ast->name == funcName
      && instr.func.usage->count > 0
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
          assert(cc.isVirtRegValid((*argsIt)->out));
          assert(cc.isVirtRegValid((*inputsIt)->out));
          if ((*argsIt)->out != (*inputsIt)->out) {
            cc.mov((*argsIt)->out, (*inputsIt)->out);
          }
          ++inputsIt;
        } else {
          if ((*argsIt)->type != VAIVEN_STATIC_TYPE_VOID) {
            assert(cc.isVirtRegValid((*argsIt)->out));
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
  if (instr.funcName == funcName && instr.func.usage->count > 0) {
    // only call the funcLabel directly if its optimized
    call = cc.call(funcLabel, sig);
  } else if (instr.func.isNative) {
    call = cc.call((uint64_t) instr.func.fptr, sig);
  } else {
    X86Gp lookup = cc.newUInt64();
    cc.mov(lookup, (uint64_t) &instr.func.fptr);
    call = cc.call(x86::ptr(lookup), sig);
  }

  for (int i = 0; i < paramc; ++i) {
    assert(cc.isVirtRegValid(instr.inputs[i]->out));
    call->setArg(i, instr.inputs[i]->out);
  }

  for (int i = paramc; i < argc; ++i) {
    call->setArg(i, voidRegs[i - paramc]);
  }

  assert(cc.isVirtRegValid(instr.out));
  call->setRet(0, instr.out);
}

void Emitter::visitTypecheckInstr(TypecheckInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

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
    cc.ja(error.stringTypeErrorLabel);
    cc.mov(checkReg.r32(), x86::dword_ptr(instr.out));
    cc.cmp(checkReg.r32(), GCABLE_TYPE_STRING);
    cc.jne(error.stringTypeErrorLabel);
    error.hasStringTypeError = true;
  }
}

void Emitter::visitBoxInstr(BoxInstr& instr) {
  if (instr.type == VAIVEN_STATIC_TYPE_DOUBLE) {
    assert(cc.isVirtRegValid(instr.inputs[0]->xmmout));

    X86Mem mem = cc.newStack(8, 8);
    cc.movsd(mem, instr.inputs[0]->xmmout);
    assert(cc.isVirtRegValid(instr.out));
    cc.mov(instr.out, mem);
    X86Gp temp = cc.newUInt64();
    cc.mov(temp, (int32_t) -1); // sign extends to all ones
    cc.xor_(instr.out, temp);
    return;
  }

  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));

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

void Emitter::visitUnboxInstr(UnboxInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.xmmout));

  X86Mem mem = cc.newStack(8, 8);
  X86Gp temp = cc.newUInt64();
  cc.mov(temp, (int32_t) -1); // sign extends to all ones
  cc.xor_(instr.inputs[0]->out, temp);
  cc.mov(mem, instr.inputs[0]->out);
  cc.movsd(instr.xmmout, mem);
}

void Emitter::visitToDoubleInstr(ToDoubleInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.xmmout));

  Label afterCheck = cc.newLabel();
  Label isDbl = cc.newLabel();
  X86Gp temp = cc.newUInt64();
  cc.mov(temp, MIN_DBL);
  cc.cmp(instr.inputs[0]->out, temp);
  cc.jae(isDbl);
  cc.mov(temp, instr.inputs[0]->out);
  cc.shr(temp, VALUE_TAG_SHIFT);
  cc.cmp(temp, INT_TAG_SHIFTED);
  cc.jne(error.doubleConvertableTypeErrorLabel);
  error.hasDoubleConvertableTypeError = true;
  cc.cvtsi2sd(instr.xmmout, instr.inputs[0]->out.r32());
  cc.jmp(afterCheck);
  cc.bind(isDbl);
  X86Mem mem = cc.newStack(8, 8);
  cc.mov(temp, (int32_t) -1); // sign extends to all ones
  cc.xor_(instr.inputs[0]->out, temp);
  cc.mov(mem, instr.inputs[0]->out);
  cc.movsd(instr.xmmout, mem);
  cc.bind(afterCheck);
}

void Emitter::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
  assert(cc.isVirtRegValid(instr.xmmout));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));

  cc.cvtsi2sd(instr.xmmout, instr.inputs[0]->out.r32());
}

void Emitter::visitAddInstr(AddInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::add, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitStrAddInstr(StrAddInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::addStrUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntAddInstr(IntAddInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out, instr.inputs[0]->out);
  }

  if (instr.hasConstRhs) {
    cc.add(instr.out.r32(), instr.constRhs);
  } else {
    cc.add(instr.out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::visitDoubleAddInstr(DoubleAddInstr& instr) {
  assert(cc.isVirtRegValid(instr.xmmout));
  assert(cc.isVirtRegValid(instr.inputs[0]->xmmout));
  assert(cc.isVirtRegValid(instr.inputs[1]->xmmout));

  if (instr.xmmout != instr.inputs[0]->xmmout) {
    cc.movsd(instr.xmmout, instr.inputs[0]->xmmout);
  }

  cc.addsd(instr.xmmout, instr.inputs[1]->xmmout);
}

void Emitter::visitSubInstr(SubInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::sub, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntSubInstr(IntSubInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstLhs || cc.isVirtRegValid(instr.inputs[1]->out));

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

void Emitter::visitDoubleSubInstr(DoubleSubInstr& instr) {
  assert(cc.isVirtRegValid(instr.xmmout));
  assert(cc.isVirtRegValid(instr.inputs[0]->xmmout));
  assert(cc.isVirtRegValid(instr.inputs[1]->xmmout));

  if (instr.xmmout != instr.inputs[0]->xmmout) {
    cc.movsd(instr.xmmout, instr.inputs[0]->xmmout);
  }

  cc.subsd(instr.xmmout, instr.inputs[1]->xmmout);
}

void Emitter::visitMulInstr(MulInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::mul, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntMulInstr(IntMulInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out.r32(), instr.inputs[0]->out.r32());
  }

  if (instr.hasConstRhs) {
    cc.imul(instr.out.r32(), instr.constRhs);
  } else {
    cc.imul(instr.out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::visitDoubleMulInstr(DoubleMulInstr& instr) {
  assert(cc.isVirtRegValid(instr.xmmout));
  assert(cc.isVirtRegValid(instr.inputs[0]->xmmout));
  assert(cc.isVirtRegValid(instr.inputs[1]->xmmout));

  if (instr.xmmout != instr.inputs[0]->xmmout) {
    cc.movsd(instr.xmmout, instr.inputs[0]->xmmout);
  }

  cc.mulsd(instr.xmmout, instr.inputs[1]->xmmout);
}

void Emitter::visitDivInstr(DivInstr& instr) {
  assert(cc.isVirtRegValid(instr.xmmout));
  assert(cc.isVirtRegValid(instr.inputs[0]->xmmout));
  assert(cc.isVirtRegValid(instr.inputs[1]->xmmout));

  if (instr.xmmout != instr.inputs[0]->xmmout) {
    cc.movsd(instr.xmmout, instr.inputs[0]->xmmout);
  }

  cc.divsd(instr.xmmout, instr.inputs[1]->xmmout);
}

void Emitter::visitNotInstr(NotInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  cc.test(instr.inputs[0]->out.r32(), instr.inputs[0]->out.r32());
  cc.setz(instr.out.r8());
}

void Emitter::visitCmpEqInstr(CmpEqInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
    && instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    CCFuncCall* call = cc.call((uint64_t) vaiven::cmpStrUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
    call->setArg(0, instr.inputs[0]->out);
    call->setArg(1, instr.inputs[1]->out);
    call->setRet(0, instr.out);
    return;
  }

  CCFuncCall* call = cc.call((uint64_t) vaiven::cmpUnboxed, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doIntCmpEqInstr(instr);
  cc.setz(instr.out);
}

void Emitter::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  cc.xor_(instr.out, instr.out);
  doDoubleCmpInstr(instr);
  cc.setz(instr.out);
}

void Emitter::visitCmpIneqInstr(CmpIneqInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.inputs[0]->type == VAIVEN_STATIC_TYPE_STRING
    && instr.inputs[1]->type == VAIVEN_STATIC_TYPE_STRING) {
    CCFuncCall* call = cc.call((uint64_t) vaiven::inverseCmpStrUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
    call->setArg(0, instr.inputs[0]->out);
    call->setArg(1, instr.inputs[1]->out);
    call->setRet(0, instr.out);
    return;
  }

  CCFuncCall* call = cc.call((uint64_t) vaiven::inverseCmpUnboxed, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doIntCmpIneqInstr(instr);
  cc.setnz(instr.out);
}

void Emitter::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  cc.xor_(instr.out, instr.out);
  doDoubleCmpInstr(instr);
  cc.setnz(instr.out);
}

void Emitter::visitCmpGtInstr(CmpGtInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::gt, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doIntCmpGtInstr(instr);
  cc.setg(instr.out);
}

void Emitter::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  cc.xor_(instr.out, instr.out);
  doDoubleCmpInstr(instr);
  cc.seta(instr.out);
}

void Emitter::visitCmpGteInstr(CmpGteInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::gte, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doIntCmpGteInstr(instr);
  cc.setge(instr.out);
}

void Emitter::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  cc.xor_(instr.out, instr.out);
  doDoubleCmpInstr(instr);
  cc.setae(instr.out);
}

void Emitter::visitCmpLtInstr(CmpLtInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::lt, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doIntCmpLtInstr(instr);
  cc.setl(instr.out);
}

void Emitter::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  cc.xor_(instr.out, instr.out);
  doDoubleCmpInstr(instr);
  cc.setb(instr.out);
}

void Emitter::visitCmpLteInstr(CmpLteInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::lte, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  if (instr.isBoxed) {
    cc.mov(instr.out, BOOL_TAG);
  } else {
    cc.xor_(instr.out, instr.out);
  }

  doIntCmpLteInstr(instr);
  cc.setle(instr.out);
}

void Emitter::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

  cc.xor_(instr.out, instr.out);
  doDoubleCmpInstr(instr);
  cc.setbe(instr.out);
}

void Emitter::doIntCmpEqInstr(IntCmpEqInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constI32Rhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doDoubleCmpInstr(Instruction& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->xmmout));
  assert(cc.isVirtRegValid(instr.inputs[1]->xmmout));

  cc.ucomisd(instr.inputs[0]->xmmout, instr.inputs[1]->xmmout);
}

void Emitter::doIntCmpIneqInstr(IntCmpIneqInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constI32Rhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doIntCmpGtInstr(IntCmpGtInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doIntCmpGteInstr(IntCmpGteInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doIntCmpLtInstr(IntCmpLtInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::doIntCmpLteInstr(IntCmpLteInstr& instr) {
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(instr.hasConstRhs || cc.isVirtRegValid(instr.inputs[1]->out));

  if (instr.hasConstRhs) {
    cc.cmp(instr.inputs[0]->out.r32(), instr.constRhs);
  } else {
    cc.cmp(instr.inputs[0]->out.r32(), instr.inputs[1]->out.r32());
  }
}

void Emitter::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::get, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));
  assert(cc.isVirtRegValid(instr.inputs[2]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::set, FuncSignature3<uint64_t, uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setArg(2, instr.inputs[2]->out);
}

void Emitter::visitListAccessInstr(ListAccessInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::listAccessUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setRet(0, instr.out);
}

void Emitter::visitListStoreInstr(ListStoreInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));
  assert(cc.isVirtRegValid(instr.inputs[2]->out));

  CCFuncCall* call = cc.call((uint64_t) vaiven::listStoreUnchecked, FuncSignature3<uint64_t, uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, instr.inputs[1]->out);
  call->setArg(2, instr.inputs[2]->out);
}

void Emitter::visitListInitInstr(ListInitInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));

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

    assert(cc.isVirtRegValid((*it)->out));
    cc.mov(x86::ptr(ptr), (*it)->out);
  }
}

void Emitter::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

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
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));
  assert(cc.isVirtRegValid(instr.inputs[2]->out));

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
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));

  X86Gp str = cc.newUInt64();
  cc.mov(str, (uint64_t) instr.property);
  CCFuncCall* call = cc.call((uint64_t) vaiven::objectAccessUnchecked, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  call->setArg(0, instr.inputs[0]->out);
  call->setArg(1, str);
  call->setRet(0, instr.out);
}

void Emitter::visitObjectStoreInstr(ObjectStoreInstr& instr) {
  assert(cc.isVirtRegValid(instr.out));
  assert(cc.isVirtRegValid(instr.inputs[0]->out));
  assert(cc.isVirtRegValid(instr.inputs[1]->out));

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
  assert(cc.isVirtRegValid(instr.inputs[0]->out));

  cc.ret(instr.inputs[0]->out);
}

void Emitter::visitUnconditionalBlockExit(UnconditionalBlockExit& exit) {
  if (exit.toGoTo != &*curBlock->next) {
    cc.jmp(exit.toGoTo->label);
  }
}

void Emitter::visitConditionalBlockExit(ConditionalBlockExit& exit) {
  switch(exit.condition->tag) {
    case INSTR_INT_CMPEQ:
      doIntCmpEqInstr(static_cast<IntCmpEqInstr&>(*exit.condition));
      cc.je(exit.toGoTo->label);
      break;
    case INSTR_INT_CMPINEQ:
      doIntCmpIneqInstr(static_cast<IntCmpIneqInstr&>(*exit.condition));
      cc.jne(exit.toGoTo->label);
      break;
    case INSTR_INT_CMPGT:
      doIntCmpGtInstr(static_cast<IntCmpGtInstr&>(*exit.condition));
      cc.jg(exit.toGoTo->label);
      break;
    case INSTR_INT_CMPGTE:
      doIntCmpGteInstr(static_cast<IntCmpGteInstr&>(*exit.condition));
      cc.jge(exit.toGoTo->label);
      break;
    case INSTR_INT_CMPLT:
      doIntCmpLtInstr(static_cast<IntCmpLtInstr&>(*exit.condition));
      cc.jl(exit.toGoTo->label);
      break;
    case INSTR_INT_CMPLTE:
      doIntCmpLteInstr(static_cast<IntCmpLteInstr&>(*exit.condition));
      cc.jle(exit.toGoTo->label);
      break;
    case INSTR_DBL_CMPEQ:
      doDoubleCmpInstr(*exit.condition);
      cc.je(exit.toGoTo->label);
      break;
    case INSTR_DBL_CMPINEQ:
      doDoubleCmpInstr(*exit.condition);
      cc.jne(exit.toGoTo->label);
      break;
    case INSTR_DBL_CMPGT:
      doDoubleCmpInstr(*exit.condition);
      cc.ja(exit.toGoTo->label);
      break;
    case INSTR_DBL_CMPGTE:
      doDoubleCmpInstr(*exit.condition);
      cc.jae(exit.toGoTo->label);
      break;
    case INSTR_DBL_CMPLT:
      doDoubleCmpInstr(*exit.condition);
      cc.jb(exit.toGoTo->label);
      break;
    case INSTR_DBL_CMPLTE:
      doDoubleCmpInstr(*exit.condition);
      cc.jbe(exit.toGoTo->label);
      break;
    case INSTR_NOT:
      cc.test(exit.condition->inputs[0]->out.r32(), exit.condition->inputs[0]->out.r32());
      cc.jz(exit.toGoTo->label);
      break;
    default:
      exit.condition->accept(*this);
      assert(cc.isVirtRegValid(exit.condition->out));
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
      if ((*it)->tag == INSTR_PHI) {
        if ((*it)->type == VAIVEN_STATIC_TYPE_DOUBLE && !(*it)->isBoxed) {
          assert(cc.isVirtRegValid((*it)->xmmout));
          assert(cc.isVirtRegValid(next->xmmout));
          if ((*it)->xmmout != next->xmmout) {
            cc.movsd((*it)->xmmout, next->xmmout);
          }
        } else {
          assert(cc.isVirtRegValid((*it)->out));
          assert(cc.isVirtRegValid(next->out));
          if ((*it)->out != next->out) {
            cc.mov((*it)->out, next->out);
          }
        }
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
