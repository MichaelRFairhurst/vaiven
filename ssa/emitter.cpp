#include "emitter.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void Emitter::visitPhiInstr(PhiInstr& instr) {
}

void Emitter::visitArgInstr(ArgInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitConstantInstr(ConstantInstr& instr) {
  if (instr.isBoxed || instr.val.isPtr()) {
    cc.mov(instr.out, instr.val.getRaw());
  } else if (instr.val.isInt() || instr.val.isBool() || instr.val.isVoid()) {
    cc.mov(instr.out.r32(), instr.val.getInt());
  } else {
    cc.mov(instr.out.r32(), instr.val.getDouble());
  }

  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitCallInstr(CallInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitTypecheckInstr(TypecheckInstr& instr) {
  X86Gp checkReg = cc.newUInt64();
  if (instr.type == VAIVEN_STATIC_TYPE_INT) {
    cc.mov(checkReg, instr.out);
    cc.shr(checkReg, VALUE_TAG_SHIFT);
    cc.cmp(checkReg, INT_TAG_SHIFTED);
    cc.jne((unsigned long long) 0); // TODO jmp to throw error
  }

  if (instr.next != NULL) instr.next->accept(*this);
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

  if (instr.next != NULL) instr.next->accept(*this);
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

  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitSubInstr(SubInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out.r32(), instr.inputs[0]->out.r32());
  }

  if (instr.hasConstLhs) {
    cc.neg(instr.out.r32());
    cc.add(instr.out.r32(), instr.constLhs);
  } else {
    cc.sub(instr.out.r32(), instr.inputs[1]->out.r32());
  }

  if (instr.next != NULL) instr.next->accept(*this);
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

  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitDivInstr(DivInstr& instr) {
  if (instr.out != instr.inputs[0]->out) {
    cc.mov(instr.out.r32(), instr.inputs[0]->out.r32());
  }

  X86Gp dummy = cc.newUInt64();
  cc.xor_(dummy, dummy);
  cc.idiv(dummy.r32(), instr.out.r32(), instr.inputs[1]->out.r32());
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitNotInstr(NotInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitCmpEqInstr(CmpEqInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitCmpGtInstr(CmpGtInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitCmpGteInstr(CmpGteInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitCmpLtInstr(CmpLtInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitCmpLteInstr(CmpLteInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitErrInstr(ErrInstr& instr) {
  if (instr.next != NULL) instr.next->accept(*this);
}

void Emitter::visitRetInstr(RetInstr& instr) {
  cc.ret(instr.inputs[0]->out);
  if (instr.next != NULL) instr.next->accept(*this);
}
