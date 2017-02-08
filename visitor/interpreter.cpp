#include "interpreter.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

//int Interpreter::interpret(Node<>& root, vector<int> args, map<string, int>* variablesMap) {
int Interpreter::interpret(Node<>& root) {
  //for(vector<int>::iterator it = args.begin(); it != args.end(); ++it) {
  //  stack.push(*it);
  //}
  //this->variablesMap = variablesMap;

  root.accept(*this);

  int val = stack.top();
  //for(int i = 0; i < args.size(); ++i) {
  //  stack.pop();
  //}
  //stack.pop();

  return val;
}

void Interpreter::visitFuncDecl(FuncDecl<>& decl) {
  // nothing
}

void Interpreter::visitExpressionStatement(ExpressionStatement<>& stmt) {
  stmt.expr->accept(*this);
}

void Interpreter::visitBlock(Block<>& block) {
  for(vector<unique_ptr<Statement<> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    if (it != block.statements.begin()) {
      stack.pop();
    }
    (*it)->accept(*this);
  }
}

void Interpreter::visitAdditionExpression(AdditionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left + right);
}
void Interpreter::visitSubtractionExpression(SubtractionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left - right);
}
void Interpreter::visitMultiplicationExpression(MultiplicationExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left * right);
}
void Interpreter::visitDivisionExpression(DivisionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  int right = stack.top(); stack.pop();
  int left = stack.top(); stack.pop();
  stack.push(left / right);
}
void Interpreter::visitIntegerExpression(IntegerExpression<>& expr) {
  stack.push(expr.value);
}
void Interpreter::visitVariableExpression(VariableExpression<>& expr) {
  int val = stack.c[(*variablesMap)[expr.id]];
  stack.push(val);
}
