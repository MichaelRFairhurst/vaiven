#include "compiler.h"

#include "../ast/all.h"

using namespace asmjit;
using namespace vaiven::visitor;

void Compiler::compile(Expression<Location>& root) {
  root.accept(*this);
  asm.ret();
}

void Compiler::visitAdditionExpression(AdditionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftExactReg = left_loc.type == LOCATION_TYPE_REG;
  bool rightExactReg = right_loc.type == LOCATION_TYPE_REG;
  bool leftTmp = !leftImm && !leftExactReg;
  bool rightTmp = !rightImm && !rightExactReg;

  if (!leftTmp && !rightTmp) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    expr.left->accept(*this);
  } else if (leftTmp) {
    expr.left->accept(*this);
    if (rightTmp) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (rightTmp) {
    expr.right->accept(*this);
    if (leftTmp) {
      // two non immediates fight over rax, required stack manip
      asm.pop(x86::rcx);
    }
  }

  if (rightImm) {
    // we already ensured that if right is imm, left is in rax
    // just add right imm
    asm.add(x86::rax, right_loc.data.imm);
  } else if (rightExactReg) {
    // we already ensured that if right is imm, left is in rax
    // just add right imm
    asm.add(x86::rax, *right_loc.data.reg);
  } else if (leftImm) {
    // then rhs is in rax 
    asm.add(x86::rax, left_loc.data.imm);
  } else if (leftExactReg) {
    // then rhs is in rax 
    asm.add(x86::rax, *left_loc.data.reg);
  } else {
    asm.add(x86::rax, x86::rcx);
  }
}
void Compiler::visitSubtractionExpression(SubtractionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftExactReg = left_loc.type == LOCATION_TYPE_REG;
  bool rightExactReg = right_loc.type == LOCATION_TYPE_REG;
  bool leftTmp = !leftImm && !leftExactReg;
  bool rightTmp = !rightImm && !rightExactReg;

  if (!leftTmp && !rightTmp) {
    // mov rax, lhsImm or exact reg
    // mul rax, rhsImm or exact reg
    expr.left->accept(*this);
  } else if (leftTmp) {
    expr.left->accept(*this);
    if (rightTmp) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (rightTmp) {
    expr.right->accept(*this);
    if (leftTmp) {
      // two non immediates fight over rax, required stack manip
      asm.pop(x86::rcx);
    }
  }

  if (rightImm) {
    // we already ensured that if right is imm, left is in rax
    // just add right imm
    asm.sub(x86::rax, right_loc.data.imm);
  } else if (rightExactReg) {
    asm.sub(x86::rax, *right_loc.data.reg);
  } else if (leftImm) {
    // then rhs is in rax. run -rhs + lhs
    asm.neg(x86::rax);
    asm.add(x86::rax, left_loc.data.imm);
  } else if (leftExactReg) {
    // then rhs is in rax. run -rhs + lhs
    asm.neg(x86::rax);
    asm.add(x86::rax, *left_loc.data.reg);
  } else {
    // rhs is in rax. run -rhs + lhs
    asm.neg(x86::rax);
    asm.add(x86::rax, x86::rcx);
  }
}
void Compiler::visitMultiplicationExpression(MultiplicationExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftExactReg = left_loc.type == LOCATION_TYPE_REG;
  bool rightExactReg = right_loc.type == LOCATION_TYPE_REG;
  bool leftTmp = !leftImm && !leftExactReg;
  bool rightTmp = !rightImm && !rightExactReg;

  if (!leftTmp && !rightTmp) {
    // mov rax, lhsImm or exact reg
    // mul rax, rhsImm or exact reg
    expr.left->accept(*this);
  } else if (leftTmp) {
    expr.left->accept(*this);
    if (rightTmp) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (rightTmp) {
    expr.right->accept(*this);
    if (leftTmp) {
      // two non immediates fight over rax, required stack manip
      asm.pop(x86::rcx);
    }
  }

  if (rightImm) {
    // we already ensured that if right is imm, left is in rax
    // just mul right imm
    asm.imul(x86::rax, right_loc.data.imm);
  } else if (rightExactReg) {
    // we already ensured that if right is imm, left is in rax
    // just mul right imm
    asm.imul(x86::rax, *right_loc.data.reg);
  } else if (leftImm) {
    // then rhs is in rax 
    asm.imul(x86::rax, left_loc.data.imm);
  } else if (leftExactReg) {
    // then rhs is in rax 
    asm.imul(x86::rax, *left_loc.data.reg);
  } else {
    asm.imul(x86::rax, x86::rcx);
  }
}
void Compiler::visitDivisionExpression(DivisionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftExactReg = left_loc.type == LOCATION_TYPE_REG;
  bool rightExactReg = right_loc.type == LOCATION_TYPE_REG;
  bool leftTmp = !leftImm && !leftExactReg;
  bool rightTmp = !rightImm && !rightExactReg;

  if (!leftTmp && !rightTmp) {
    // mov rax, lhsImm or exact reg
    // mul rax, rhsImm or exact reg
    expr.left->accept(*this);
  } else if (leftTmp) {
    expr.left->accept(*this);
    if (rightTmp) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (rightTmp) {
    expr.right->accept(*this);
    if (leftTmp) {
      // two non immediates fight over rax, required stack manip
      asm.mov(x86::rcx, x86::rax);
      asm.pop(x86::rax);
    }
  }

  // clear upper 64 bits
  asm.xor_(x86::rdx, x86::rdx);

  if (rightImm) {
    // we already ensured that if right is imm, left is in rax
    // can't idiv by an immediate, so first load it into rcx
    asm.mov(x86::rcx, right_loc.data.imm);
    asm.idiv(x86::rcx);
  } else if (rightExactReg) {
    asm.idiv(*right_loc.data.reg);
  } else if (leftImm) {
    // then rhs is in rax. Want it in rcx
    asm.mov(x86::rcx, x86::rax);
    asm.mov(x86::rax, left_loc.data.imm);
    asm.idiv(x86::rcx);
  } else if (leftExactReg) {
    // then rhs is in rax. Want it in rcx
    asm.mov(x86::rcx, x86::rax);
    asm.mov(x86::rax, *left_loc.data.reg);
    asm.idiv(x86::rcx);
  } else {
    asm.idiv(x86::rcx);
  }
}

void Compiler::visitIntegerExpression(IntegerExpression<Location>& expr) {
  // should only happen when in a stmt by itself
  asm.mov(x86::rax, expr.value);
}

void Compiler::visitVariableExpression(VariableExpression<Location>& expr) {
  asm.mov(x86::rax, *expr.resolvedData.data.reg);
}
