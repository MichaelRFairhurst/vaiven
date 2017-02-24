#include "ssa_builder.h"

#include "../ast/all.h"

using namespace vaiven::visitor;
using namespace vaiven::ssa;

void SsaBuilder::emit(Instruction* next) {
  if (cur == NULL) {
    writePoint = next;
    first = next;
    cur = next;
  } else {
    writePoint->next = next;
    writePoint = next;
    cur = next;
  }
}

void SsaBuilder::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  throw "not supported";
}

void SsaBuilder::visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
  Instruction* retVal = cur;
  emit(new RetInstr(retVal));
}

void SsaBuilder::visitVarDecl(VarDecl<TypedLocationInfo>& varDecl) {
  varDecl.expr->accept(*this);
  scope.put(varDecl.varname, cur);
}

void SsaBuilder::visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr) {
  throw "not yet supported";
}

void SsaBuilder::visitFuncDecl(FuncDecl<TypedLocationInfo>& decl) {
  for (int i = 0; i < decl.args.size(); ++i) {
    ArgInstr* arg = new ArgInstr(i, usageInfo.argShapes[i].getStaticType());
    emit(arg); // part of the instruction stream for ownership purposes
    scope.put(decl.args[i], arg);
  }

  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
  }

  if (cur != NULL) {
    emit(new RetInstr(cur));
  }
}

void SsaBuilder::visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
}

void SsaBuilder::visitBlock(Block<TypedLocationInfo>& block) {
  throw "not supported";
}

void SsaBuilder::visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new AddInstr(lhs, rhs));
}

void SsaBuilder::visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new SubInstr(lhs, rhs));
}

void SsaBuilder::visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new MulInstr(lhs, rhs));
}

void SsaBuilder::visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new DivInstr(lhs, rhs));
}

void SsaBuilder::visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr) {
  emit(new ConstantInstr(Value(expr.value)));
}

void SsaBuilder::visitVariableExpression(VariableExpression<TypedLocationInfo>& expr) {
  if (scope.contains(expr.id)) {
    // don't emit the instruction, just place it as the cur so other instructions
    // can reference it
    cur = scope.get(expr.id);
  } else {
    emit(new ErrInstr());
  }
}

void SsaBuilder::visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr) {
  emit(new ConstantInstr(Value(expr.value)));
}

void SsaBuilder::visitNotExpression(NotExpression<TypedLocationInfo>& expr) {
  expr.expr->accept(*this);
  Instruction* inner = cur;
  emit(new NotInstr(inner));
}

void SsaBuilder::visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpIneqInstr(lhs, rhs));
}

void SsaBuilder::visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpEqInstr(lhs, rhs));
}

void SsaBuilder::visitGtExpression(GtExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpGtInstr(lhs, rhs));
}

void SsaBuilder::visitGteExpression(GteExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpGteInstr(lhs, rhs));
}

void SsaBuilder::visitLtExpression(LtExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpLtInstr(lhs, rhs));
}

void SsaBuilder::visitLteExpression(LteExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpLteInstr(lhs, rhs));
}