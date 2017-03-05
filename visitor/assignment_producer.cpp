#include "assignment_producer.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

void AssignmentProducer::visitIfStatement(IfStatement<>& stmt) {
  throw "not supported";
}

void AssignmentProducer::visitReturnStatement(ReturnStatement<>& stmt) {
  throw "not supported";
}

void AssignmentProducer::visitVarDecl(VarDecl<>& varDecl) {
  throw "not supported";
}

void AssignmentProducer::visitFuncCallExpression(FuncCallExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitFuncDecl(FuncDecl<>& decl) {
  throw "not supported";
}

void AssignmentProducer::visitExpressionStatement(ExpressionStatement<>& stmt) {
  throw "not supported";
}

void AssignmentProducer::visitBlock(Block<>& block) {
  throw "not supported";
}

void AssignmentProducer::visitAssignmentExpression(AssignmentExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitAdditionExpression(AdditionExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitSubtractionExpression(SubtractionExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitMultiplicationExpression(MultiplicationExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitDivisionExpression(DivisionExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitIntegerExpression(IntegerExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitVariableExpression(VariableExpression<>& expr) {
  result.reset(new AssignmentExpression<>(expr.id, std::move(rhs)));
}

void AssignmentProducer::visitBoolLiteral(BoolLiteral<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitNotExpression(NotExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitInequalityExpression(InequalityExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitEqualityExpression(EqualityExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitGtExpression(GtExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitGteExpression(GteExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitLtExpression(LtExpression<>& expr) {
  throw "not supported";
}

void AssignmentProducer::visitLteExpression(LteExpression<>& expr) {
  throw "not supported";
}
