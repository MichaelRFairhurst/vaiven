#include "usage_applier.h"

#include "../ast/all.h"

using namespace vaiven::visitor;

void UsageApplier::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  stmt.condition->accept(*this);

  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.trueStatements.begin();
      it != stmt.trueStatements.end();
      ++it) {
    (*it)->accept(*this);
  }
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.falseStatements.begin();
      it != stmt.falseStatements.end();
      ++it) {
    (*it)->accept(*this);
  }
}

void UsageApplier::visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
}

void UsageApplier::visitVarDecl(VarDecl<TypedLocationInfo>& varDecl) {
  varDecl.expr->accept(*this);
}

void UsageApplier::visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr) {
  for(vector<unique_ptr<Expression<TypedLocationInfo> > >::iterator it = expr.parameters.begin();
      it != expr.parameters.end();
      ++it) {
    (*it)->accept(*this);
  }
}

void UsageApplier::visitFuncDecl(FuncDecl<TypedLocationInfo>& decl) {
  for(int i = 0; i < decl.args.size(); ++i) {
    argIndexes[decl.args[i]] = i;
  }

  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
}

void UsageApplier::visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
}

void UsageApplier::visitBlock(Block<TypedLocationInfo>& block) {
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
}

void UsageApplier::visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr) {
  expr.expr->accept(*this);
}

void UsageApplier::visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}
void UsageApplier::visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}
void UsageApplier::visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}
void UsageApplier::visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}
void UsageApplier::visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr) {
}
void UsageApplier::visitVariableExpression(VariableExpression<TypedLocationInfo>& expr) {
  if (argIndexes.find(expr.id) != argIndexes.end()) {
    int i = argIndexes[expr.id];
    if (usageInfo.argShapes[i].isPureInt()) {
      expr.resolvedData.type = VAIVEN_STATIC_TYPE_INT;
    } else if (usageInfo.argShapes[i].isPureVoid()) {
      expr.resolvedData.type = VAIVEN_STATIC_TYPE_VOID;
    } else if (usageInfo.argShapes[i].isPureDouble()) {
      expr.resolvedData.type = VAIVEN_STATIC_TYPE_DOUBLE;
    } else if (usageInfo.argShapes[i].isPureObject()) {
      expr.resolvedData.type = VAIVEN_STATIC_TYPE_DOUBLE;
    } else if (usageInfo.argShapes[i].isPureBool()) {
      expr.resolvedData.type = VAIVEN_STATIC_TYPE_BOOL;
    }
  }
}

void UsageApplier::visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr) {
}

void UsageApplier::visitNotExpression(NotExpression<TypedLocationInfo>& expr) {
  expr.expr->accept(*this);
}

void UsageApplier::visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}

void UsageApplier::visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}

void UsageApplier::visitGtExpression(GtExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}

void UsageApplier::visitGteExpression(GteExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}

void UsageApplier::visitLtExpression(LtExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}

void UsageApplier::visitLteExpression(LteExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
}
