#include "compiler.h"

#include "../ast/all.h"

using namespace asmjit;
using namespace vaiven::visitor;

void Compiler::compile(Node<Location>& root, int argc) {
  if (argc > 6) {
    asm.push(x86::rbp);
    asm.mov(x86::rbp, x86::rsp);
    root.accept(*this);
    asm.pop(x86::rbp);
  } else {
    root.accept(*this);
  }
  asm.ret();
}

void Compiler::visitFuncDecl(FuncDecl<Location>& decl) {
  // TODO compile this
}

void Compiler::visitExpressionStatement(ExpressionStatement<Location>& stmt) {
  stmt.expr->accept(*this);
}

void Compiler::visitBlock(Block<Location>& block) {
  for(vector<unique_ptr<Statement<Location> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
}


void Compiler::visitAdditionExpression(AdditionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftTmp = left_loc.type == LOCATION_TYPE_SPILLED;
  bool rightTmp = right_loc.type == LOCATION_TYPE_SPILLED;

  if (!leftTmp && rightTmp) {
    expr.right->accept(*this);
    if (leftImm) {
      asm.add(x86::rax, left_loc.data.imm);
    } else {
      const X86Gp* argreg = left_loc.getReg();
      if (argreg != NULL) {
        asm.add(x86::rax, *argreg);
      } else {
        asm.add(x86::rax, right_loc.getArgPtr());
      }
    }
  } else {
    expr.left->accept(*this);
    if (rightTmp) {
      asm.push(x86::rax);
      expr.right->accept(*this);
      asm.pop(x86::rcx);
      asm.add(x86::rax, x86::rcx);
    } else if (rightImm) {
      asm.add(x86::rax, right_loc.data.imm);
    } else {
      const X86Gp* argreg = right_loc.getReg();
      if (argreg != NULL) {
        asm.add(x86::rax, *argreg);
      } else {
        asm.add(x86::rax, right_loc.getArgPtr());
      }
    }
  }
}
void Compiler::visitSubtractionExpression(SubtractionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftTmp = left_loc.type == LOCATION_TYPE_SPILLED;
  bool rightTmp = right_loc.type == LOCATION_TYPE_SPILLED;

  if (!leftTmp && rightTmp) {
    expr.right->accept(*this);
    // rhs is in rax, but we want lhs -= rhs. So instead rhs = -rhs + lhs
    asm.neg(x86::rax);
    if (leftImm) {
      asm.add(x86::rax, left_loc.data.imm);
    } else {
      const X86Gp* argreg = left_loc.getReg();
      if (argreg != NULL) {
        asm.add(x86::rax, *argreg);
      } else {
        asm.add(x86::rax, right_loc.getArgPtr());
      }
    }
  } else {
    if (rightTmp) {
      expr.right->accept(*this);
      asm.push(x86::rax);
      expr.left->accept(*this);
      asm.pop(x86::rcx);
      asm.sub(x86::rax, x86::rcx);
    } else if (rightImm) {
      expr.left->accept(*this);
      asm.sub(x86::rax, right_loc.data.imm);
    } else {
      expr.left->accept(*this);
      const X86Gp* argreg = right_loc.getReg();
      if (argreg != NULL) {
        asm.sub(x86::rax, *argreg);
      } else {
        asm.sub(x86::rax, right_loc.getArgPtr());
      }
    }
  }
}
void Compiler::visitMultiplicationExpression(MultiplicationExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftTmp = left_loc.type == LOCATION_TYPE_SPILLED;
  bool rightTmp = right_loc.type == LOCATION_TYPE_SPILLED;

  if (!leftTmp && rightTmp) {
    expr.right->accept(*this);
    if (leftImm) {
      asm.imul(x86::rax, left_loc.data.imm);
    } else {
      const X86Gp* argreg = left_loc.getReg();
      if (argreg != NULL) {
        asm.imul(x86::rax, *argreg);
      } else {
        asm.imul(x86::rax, right_loc.getArgPtr());
      }
    }
  } else {
    expr.left->accept(*this);
    if (rightTmp) {
      asm.push(x86::rax);
      expr.right->accept(*this);
      asm.pop(x86::rcx);
      asm.imul(x86::rax, x86::rcx);
    } else if (rightImm) {
      asm.imul(x86::rax, right_loc.data.imm);
    } else {
      const X86Gp* argreg = right_loc.getReg();
      if (argreg != NULL) {
        asm.imul(x86::rax, *argreg);
      } else {
        asm.imul(x86::rax, right_loc.getArgPtr());
      }
    }
  }
}
void Compiler::visitDivisionExpression(DivisionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;
  bool leftTmp = left_loc.type == LOCATION_TYPE_SPILLED;
  bool rightTmp = right_loc.type == LOCATION_TYPE_SPILLED;

  // division takes no immediates or effective addrs.
  // These only set up rax and the div reg (usually rcx)
  X86Gp divReg = x86::rcx;
  if (!leftTmp && rightTmp) {
    expr.right->accept(*this);
    asm.mov(divReg, x86::rax);
    if (leftImm) {
      asm.mov(x86::rax, left_loc.data.imm);
    } else {
      const X86Gp* argreg = left_loc.getReg();
      if (argreg != NULL) {
        asm.mov(x86::rax, right_loc.getArgPtr());
      } else {
        asm.mov(x86::rax, right_loc.getArgPtr());
      }
    }
  } else {
    if (rightTmp) {
      expr.right->accept(*this);
      asm.push(x86::rax);
      expr.left->accept(*this);
      asm.pop(x86::rcx);
    } else if (rightImm) {
      expr.left->accept(*this);
      asm.mov(x86::rcx, right_loc.data.imm);
    } else {
      expr.left->accept(*this);
      const X86Gp* argreg = right_loc.getReg();
      if (argreg != NULL) {
        divReg = *argreg;
      } else {
        asm.mov(x86::rcx, right_loc.getArgPtr());
      }
    }
  }

  // TODO only do this when rdx is in use
  // save variable 4
  asm.push(x86::rdx);
  // clear high div bits
  asm.xor_(x86::rdx, x86::rdx);
  // run div
  asm.idiv(x86::rcx);
  // restore variable 5
  asm.pop(x86::rdx);
}

void Compiler::visitIntegerExpression(IntegerExpression<Location>& expr) {
  // should only happen when in a stmt by itself
  asm.mov(x86::rax, expr.value);
}

void Compiler::visitVariableExpression(VariableExpression<Location>& expr) {
  const X86Gp* reg = expr.resolvedData.getReg();
  if (reg != NULL) {
    asm.mov(x86::rax, *reg);
  } else {
    asm.mov(x86::rax, expr.resolvedData.getArgPtr());
  }
}
