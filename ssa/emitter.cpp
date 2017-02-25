#include "emitter.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void Emitter::visitPhiInstr(PhiInstr& instr) {
}

void Emitter::visitArgInstr(ArgInstr& instr) {
}

void Emitter::visitConstantInstr(ConstantInstr& instr) {
  if (instr.isBoxed || instr.val.isPtr()) {
    cc.mov(instr.out, instr.val.getRaw());
  } else if (instr.val.isInt() || instr.val.isBool() || instr.val.isVoid()) {
    cc.mov(instr.out.r32(), instr.val.getInt());
  } else {
    cc.mov(instr.out.r32(), instr.val.getDouble());
  }
}

void Emitter::visitCallInstr(CallInstr& instr) {
  // TODO recursion
  if (funcs.funcs.find(instr.funcName) == funcs.funcs.end()) {
    throw "func not known";
  }
  // TODO check arg counts

  uint8_t sigArgs[instr.inputs.size()];
  for (int i = 0; i < instr.inputs.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, instr.inputs.size());

  CCFuncCall* call;
  //if (instr.funcName == curFuncName) {
  //  call = cc.call(curFunc->getLabel(), sig);
  //} else {
    X86Gp lookup = cc.newUInt64();
    cc.mov(lookup, (unsigned long long) &funcs.funcs[instr.funcName]->fptr);
    cc.mov(lookup, x86::ptr(lookup));
    call = cc.call(lookup, sig);
  //}

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
    cc.jne((unsigned long long) 0); // TODO jmp to throw error
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
}

void Emitter::visitCmpEqInstr(CmpEqInstr& instr) {
}

void Emitter::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void Emitter::visitCmpGtInstr(CmpGtInstr& instr) {
}

void Emitter::visitCmpGteInstr(CmpGteInstr& instr) {
}

void Emitter::visitCmpLtInstr(CmpLtInstr& instr) {
}

void Emitter::visitCmpLteInstr(CmpLteInstr& instr) {
}

void Emitter::visitErrInstr(ErrInstr& instr) {
}

void Emitter::visitRetInstr(RetInstr& instr) {
  cc.ret(instr.inputs[0]->out);
}
