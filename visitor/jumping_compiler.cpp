#include "jumping_compiler.h"

using namespace vaiven::visitor;
using namespace vaiven::ast;

void JumpingCompiler::visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr) {
  compiler.visitAssignmentExpression(expr);
}

void JumpingCompiler::visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr) {
  compiler.visitAdditionExpression(expr);
}

void JumpingCompiler::visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr) {
  compiler.visitSubtractionExpression(expr);
}

void JumpingCompiler::visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr) {
  compiler.visitMultiplicationExpression(expr);
}

void JumpingCompiler::visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr) {
  compiler.visitDivisionExpression(expr);
}

void JumpingCompiler::visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr) {
  compiler.visitIntegerExpression(expr);
}

void JumpingCompiler::visitStringExpression(StringExpression<TypedLocationInfo>& expr) {
  compiler.visitStringExpression(expr);
}

void JumpingCompiler::visitVariableExpression(VariableExpression<TypedLocationInfo>& expr) {
  compiler.visitVariableExpression(expr);
}

void JumpingCompiler::visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr) {
  compiler.visitBoolLiteral(expr);
}

void JumpingCompiler::visitNotExpression(NotExpression<TypedLocationInfo>& expr) {
  jmpFalse = !jmpFalse;
  expr.expr->accept(*this);
}

void JumpingCompiler::visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr) {
  if (!compiler.doCmpEqualityExpression(*expr.left, *expr.right, true)) {
    return;
  }
  if (jmpFalse) {
    cc.jne(label);
  } else {
    cc.je(label);
  }
  didJmp = true;
}

void JumpingCompiler::visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr) {
  if (!compiler.doCmpEqualityExpression(*expr.left, *expr.right, false)) {
    return;
  }
  if (jmpFalse) {
    cc.je(label);
  } else {
    cc.jne(label);
  }
  didJmp = true;
}

void JumpingCompiler::visitGtExpression(GtExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type == LOCATION_TYPE_IMM && right_loc.type != LOCATION_TYPE_IMM;
  compiler.doCmpIntExpression(*expr.left, *expr.right);
  if (jmpFalse) {
    if (backwardsCmp) {
      cc.jge(label);
    } else {
      cc.jle(label);
    }
  } else {
    if (backwardsCmp) {
      cc.jl(label);
    } else {
      cc.jg(label);
    }
  }
  didJmp = true;
}

void JumpingCompiler::visitGteExpression(GteExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type == LOCATION_TYPE_IMM && right_loc.type != LOCATION_TYPE_IMM;
  compiler.doCmpIntExpression(*expr.left, *expr.right);
  if (jmpFalse) {
    if (backwardsCmp) {
      cc.jg(label);
    } else {
      cc.jl(label);
    }
  } else {
    if (backwardsCmp) {
      cc.jle(label);
    } else {
      cc.jge(label);
    }
  }
  didJmp = true;
}

void JumpingCompiler::visitLtExpression(LtExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type == LOCATION_TYPE_IMM && right_loc.type != LOCATION_TYPE_IMM;
  compiler.doCmpIntExpression(*expr.left, *expr.right);
  if (jmpFalse) {
    if (backwardsCmp) {
      cc.jle(label);
    } else {
      cc.jge(label);
    }
  } else {
    if (backwardsCmp) {
      cc.jg(label);
    } else {
      cc.jl(label);
    }
  }
  didJmp = true;
}

void JumpingCompiler::visitLteExpression(LteExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type == LOCATION_TYPE_IMM && right_loc.type != LOCATION_TYPE_IMM;
  compiler.doCmpIntExpression(*expr.left, *expr.right);
  if (jmpFalse) {
    if (backwardsCmp) {
      cc.jl(label);
    } else {
      cc.jg(label);
    }
  } else {
    if (backwardsCmp) {
      cc.jge(label);
    } else {
      cc.jle(label);
    }
  }
  didJmp = true;
}

void JumpingCompiler::visitListLiteralExpression(ListLiteralExpression<TypedLocationInfo>& expr) {
  compiler.visitListLiteralExpression(expr);
}

void JumpingCompiler::visitDynamicAccessExpression(DynamicAccessExpression<TypedLocationInfo>& expr) {
  compiler.visitDynamicAccessExpression(expr);
}

void JumpingCompiler::visitDynamicStoreExpression(DynamicStoreExpression<TypedLocationInfo>& expr) {
  compiler.visitDynamicStoreExpression(expr);
}

void JumpingCompiler::visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr) {
  compiler.visitFuncCallExpression(expr);
}

void JumpingCompiler::visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt) {
  compiler.visitExpressionStatement(stmt);
}

void JumpingCompiler::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  compiler.visitIfStatement(stmt);
}

void JumpingCompiler::visitForCondition(ForCondition<TypedLocationInfo>& stmt) {
  compiler.visitForCondition(stmt);
}

void JumpingCompiler::visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt) {
  compiler.visitReturnStatement(stmt);
}

void JumpingCompiler::visitBlock(Block<TypedLocationInfo>& expr) {
  compiler.visitBlock(expr);
}

void JumpingCompiler::visitFuncDecl(FuncDecl<TypedLocationInfo>& funcDecl) {
  compiler.visitFuncDecl(funcDecl);
}

void JumpingCompiler::visitVarDecl(VarDecl<TypedLocationInfo>& varDecl) {
  compiler.visitVarDecl(varDecl);
}
