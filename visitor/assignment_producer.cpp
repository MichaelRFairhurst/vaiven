#include "assignment_producer.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

void AssignmentProducer::visitIfStatement(IfStatement<>& stmt) {
}

void AssignmentProducer::visitForCondition(ForCondition<>& stmt) {
}

void AssignmentProducer::visitReturnStatement(ReturnStatement<>& stmt) {
}

void AssignmentProducer::visitVarDecl(VarDecl<>& varDecl) {
}

void AssignmentProducer::visitListLiteralExpression(ListLiteralExpression<>& expr) {
}

void AssignmentProducer::visitDynamicAccessExpression(DynamicAccessExpression<>& expr) {
  result.reset(new DynamicStoreExpression<>(
      std::move(expr.subject),
      std::move(expr.property),
      std::move(rhs),
      preAssignmentOp));
}

void AssignmentProducer::visitDynamicStoreExpression(DynamicStoreExpression<>& expr) {
}

void AssignmentProducer::visitStaticAccessExpression(StaticAccessExpression<>& expr) {
  result.reset(new StaticStoreExpression<>(
      std::move(expr.subject),
      expr.property,
      std::move(rhs),
      preAssignmentOp));
}

void AssignmentProducer::visitStaticStoreExpression(StaticStoreExpression<>& expr) {
}

void AssignmentProducer::visitFuncCallExpression(FuncCallExpression<>& expr) {
}

void AssignmentProducer::visitFuncDecl(FuncDecl<>& decl) {
}

void AssignmentProducer::visitExpressionStatement(ExpressionStatement<>& stmt) {
}

void AssignmentProducer::visitBlock(Block<>& block) {
}

void AssignmentProducer::visitAssignmentExpression(AssignmentExpression<>& expr) {
}

void AssignmentProducer::visitAdditionExpression(AdditionExpression<>& expr) {
}

void AssignmentProducer::visitSubtractionExpression(SubtractionExpression<>& expr) {
}

void AssignmentProducer::visitMultiplicationExpression(MultiplicationExpression<>& expr) {
}

void AssignmentProducer::visitDivisionExpression(DivisionExpression<>& expr) {
}

void AssignmentProducer::visitIntegerExpression(IntegerExpression<>& expr) {
}

void AssignmentProducer::visitStringExpression(StringExpression<>& expr) {
}

void AssignmentProducer::visitVariableExpression(VariableExpression<>& expr) {
  result.reset(new AssignmentExpression<>(expr.id, std::move(rhs), preAssignmentOp));
}

void AssignmentProducer::visitBoolLiteral(BoolLiteral<>& expr) {
}

void AssignmentProducer::visitNotExpression(NotExpression<>& expr) {
}

void AssignmentProducer::visitInequalityExpression(InequalityExpression<>& expr) {
}

void AssignmentProducer::visitEqualityExpression(EqualityExpression<>& expr) {
}

void AssignmentProducer::visitGtExpression(GtExpression<>& expr) {
}

void AssignmentProducer::visitGteExpression(GteExpression<>& expr) {
}

void AssignmentProducer::visitLtExpression(LtExpression<>& expr) {
}

void AssignmentProducer::visitLteExpression(LteExpression<>& expr) {
}
