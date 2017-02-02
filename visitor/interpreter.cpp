#include "interpreter.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

void Interpreter::visitAdditionExpression(AdditionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left + right);
}
void Interpreter::visitSubtractionExpression(SubtractionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left - right);
}
void Interpreter::visitMultiplicationExpression(MultiplicationExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left * right);
}
void Interpreter::visitDivisionExpression(DivisionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left / right);
}
void Interpreter::visitIntegerExpression(IntegerExpression<bool>& expr) {
  stack.push(expr.value);
}
void Interpreter::visitVariableExpression(VariableExpression<bool>& expr) {
  stack.push(1000);
}
