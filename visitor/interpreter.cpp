#include "interpreter.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

//int Interpreter::interpret(Node<bool>& root, vector<int> args, map<string, int>* variablesMap) {
int Interpreter::interpret(Node<bool>& root) {
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

void Interpreter::visitFuncDecl(FuncDecl<bool>& decl) {
  // nothing
}

void Interpreter::visitExpressionStatement(ExpressionStatement<bool>& stmt) {
  stmt.expr->accept(*this);
}

void Interpreter::visitBlock(Block<bool>& block) {
  for(vector<unique_ptr<Statement<bool> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    if (it != block.statements.begin()) {
      stack.pop();
    }
    (*it)->accept(*this);
  }
}

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
  int val = stack.c[(*variablesMap)[expr.id]];
  stack.push(val);
}
