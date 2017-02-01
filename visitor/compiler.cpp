#include "compiler.h"

#include "../ast/all.h"

using namespace asmjit;
using namespace vaiven::visitor;

void Compiler::visitAdditionExpression(AdditionExpression<Location>& expr) {
  bool leftImm = expr.left->resolvedData.type == LOCATION_TYPE_IMM;
  bool rightImm = expr.right->resolvedData.type == LOCATION_TYPE_IMM;

  // if leftImm && !rightImm, no stack manipulation
  if (leftImm && rightImm) {
    // mov rax, lhsImm
    // add rax, rhsImm
    expr.left->accept(*this);
  } else if (!leftImm) {
    expr.left->accept(*this);
    if (!rightImm) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (!rightImm) {
    expr.right->accept(*this);
    if (!leftImm) {
      // two non immediates fight over rax, required stack manip
      asm.pop(x86::rcx);
    }
  }

  if (rightImm) {
    // we already ensured that if right is imm, left is in rax
    // just add right imm
    asm.add(x86::rax, expr.right->resolvedData.data.imm);
  } else if (leftImm) {
    // then rhs is in rax 
    asm.add(x86::rax, expr.left->resolvedData.data.imm);
  } else {
    asm.add(x86::rax, x86::rcx);
  }
}
void Compiler::visitSubtractionExpression(SubtractionExpression<Location>& expr) {
  bool leftImm = expr.left->resolvedData.type == LOCATION_TYPE_IMM;
  bool rightImm = expr.right->resolvedData.type == LOCATION_TYPE_IMM;

  if (leftImm && rightImm) {
    // mov rax, lhsImm
    // add rax, rhsImm
    expr.left->accept(*this);
  } else if (!leftImm) {
    expr.left->accept(*this);
    if (!rightImm) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (!rightImm) {
    expr.right->accept(*this);
    if (!leftImm) {
      // two non immediates fight over rax, required stack manip
      asm.pop(x86::rcx);
    }
  }

  if (rightImm) {
    // we already ensured that if right is imm, left is in rax
    // just add right imm
    asm.sub(x86::rax, expr.right->resolvedData.data.imm);
  } else if (leftImm) {
    // then rhs is in rax. run -rhs + lhs
    asm.neg(x86::rax);
    asm.add(x86::rax, expr.left->resolvedData.data.imm);
  } else {
    // rhs is in rax. run -rhs + lhs
    asm.neg(x86::rax);
    asm.add(x86::rax, x86::rcx);
  }
}
void Compiler::visitMultiplicationExpression(MultiplicationExpression<Location>& expr) {
  bool leftImm = expr.left->resolvedData.type == LOCATION_TYPE_IMM;
  bool rightImm = expr.right->resolvedData.type == LOCATION_TYPE_IMM;

  if (leftImm && rightImm) {
    // mov rax, lhsImm
    // add rax, rhsImm
    expr.left->accept(*this);
  } else if (!leftImm) {
    expr.left->accept(*this);
    if (!rightImm) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (!rightImm) {
    expr.right->accept(*this);
    if (!leftImm) {
      // two non immediates fight over rax, required stack manip
      asm.pop(x86::rcx);
    }
  }

  if (rightImm) {
    // we already ensured that if right is imm, left is in rax
    // just add right imm
    asm.imul(x86::rax, expr.right->resolvedData.data.imm);
  } else if (leftImm) {
    asm.imul(x86::rax, expr.left->resolvedData.data.imm);
  } else {
    asm.imul(x86::rax, x86::rcx);
  }
}
void Compiler::visitDivisionExpression(DivisionExpression<Location>& expr) {
  bool leftImm = expr.left->resolvedData.type == LOCATION_TYPE_IMM;
  bool rightImm = expr.right->resolvedData.type == LOCATION_TYPE_IMM;

  if (leftImm && rightImm) {
    // mov rax, lhsImm
    // add rax, rhsImm
    expr.left->accept(*this);
  } else if (!leftImm) {
    expr.left->accept(*this);
    if (!rightImm) {
      // now rax holds lhs, but calcing rhs will clobber it
      asm.push(x86::rax);
    }
  }

  if (!rightImm) {
    expr.right->accept(*this);
    if (!leftImm) {
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
    asm.mov(x86::rcx, expr.right->resolvedData.data.imm);
    asm.idiv(x86::rcx);
  } else if (leftImm) {
    // then rhs is in rax. Want it in rcx
    asm.mov(x86::rcx, x86::rax);
    asm.mov(x86::rax, expr.left->resolvedData.data.imm);
    asm.idiv(x86::rcx);
  } else {
    asm.idiv(x86::rcx);
  }
}

void Compiler::visitIntegerExpression(IntegerExpression<Location>& expr) {
  // should only happen when in a stmt by itself
  asm.mov(x86::rax, expr.value); // and update
}
