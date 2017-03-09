#include "compiler.h"

#include "../ast/all.h"

using namespace asmjit;
using namespace vaiven::visitor;

void Compiler::compile(Node<TypedLocationInfo>& root) {
  root.accept(*this);
}

void Compiler::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  Label lfalse = asm.newLabel();
  Label lafter = asm.newLabel();
  stmt.condition->accept(*this);

  //typecheckBool(x86::rax, stmt.condition->resolvedData);
  asm.cmp(x86::eax, 0);
  asm.je(lfalse);

  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.trueStatements.begin();
      it != stmt.trueStatements.end();
      ++it) {
    (*it)->accept(*this);
  }
  asm.jmp(lafter);
  asm.bind(lfalse);
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.falseStatements.begin();
      it != stmt.falseStatements.end();
      ++it) {
    (*it)->accept(*this);
  }
  asm.bind(lafter);
}

void Compiler::visitForCondition(ForCondition<TypedLocationInfo>& stmt) {
  throw "not supported";
}

void Compiler::visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
  asm.ret();
}

void Compiler::visitVarDecl(VarDecl<TypedLocationInfo>& varDecl) {
  throw "not supported";
}

void Compiler::visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr) {
  throw "not yet supported";
}

void Compiler::visitFuncDecl(FuncDecl<TypedLocationInfo>& decl) {
  if (decl.args.size() > 6) {
    asm.push(x86::rbp);
    asm.mov(x86::rbp, x86::rsp);
  }

  //curFuncName = decl.name;

  // allocate a variably sized FunctionUsage with room for shapes
  void* usageMem = malloc(sizeof(FunctionUsage) + sizeof(ArgumentShape) * decl.args.size());
  unique_ptr<FunctionUsage> usage(new (usageMem) FunctionUsage());

  funcs.prepareFunc(decl.name, decl.args.size(), std::move(usage), &decl);
  //generateTypeShapePrelog(decl, &*usage);

  //typeErrorLabel = asm.newLabel();

  TypedLocationInfo endType;
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
    endType = (*it)->resolvedData;
  }

  if (endType.type == VAIVEN_STATIC_TYPE_VOID) {
    asm.mov(x86::rax, Value().getRaw());
  }

  if (decl.args.size() > 6) {
    asm.mov(x86::rsp, x86::rbp);
    asm.pop(x86::rbp);
  }

  asm.ret();

  //generateOptimizeProlog(decl, sig);
  //generateTypeErrorProlog();

  funcs.finalizeFunc(decl.name, &codeHolder);
}

void Compiler::visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
}

void Compiler::visitBlock(Block<TypedLocationInfo>& block) {
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
}

void Compiler::visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr) {
}

void Compiler::visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
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
void Compiler::visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
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
void Compiler::visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
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
void Compiler::visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
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

void Compiler::visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr) {
  // should only happen when in a stmt by itself
  asm.mov(x86::rax, expr.value);
}

void Compiler::visitVariableExpression(VariableExpression<TypedLocationInfo>& expr) {
  const X86Gp* reg = expr.resolvedData.location.getReg();
  if (reg != NULL) {
    asm.mov(x86::rax, *reg);
  } else {
    asm.mov(x86::rax, expr.resolvedData.location.getArgPtr());
  }
}

void Compiler::visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr) {
}

void Compiler::visitNotExpression(NotExpression<TypedLocationInfo>& expr) {

}

void Compiler::visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr) {

}
void Compiler::visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr) {
}
void Compiler::visitGtExpression(GtExpression<TypedLocationInfo>& expr) {
}
void Compiler::visitGteExpression(GteExpression<TypedLocationInfo>& expr) {
}
void Compiler::visitLtExpression(LtExpression<TypedLocationInfo>& expr) {
}
void Compiler::visitLteExpression(LteExpression<TypedLocationInfo>& expr) {
}
