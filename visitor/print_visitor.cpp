#include "print_visitor.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

void PrintVisitor::visitAdditionExpression(AdditionExpression<bool>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "+";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitSubtractionExpression(SubtractionExpression<bool>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "-";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitMultiplicationExpression(MultiplicationExpression<bool>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "*";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitDivisionExpression(DivisionExpression<bool>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "/";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitIntegerExpression(IntegerExpression<bool>& expr) {
  cout << expr.value;
}
