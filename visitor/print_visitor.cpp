#include "print_visitor.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

void PrintVisitor::visitFuncDecl(FuncDecl<>& decl) {
  cout << "fn " << decl.name << " of ";
  for (vector<string>::iterator it = decl.args.begin();
      it != decl.args.end();
      ++it) {
    cout << *it << ", ";
  }
  cout << "is" << std::endl;
  for(vector<unique_ptr<Statement<> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
  cout << std::endl << "end" << std::endl;
}

void PrintVisitor::visitExpressionStatement(ExpressionStatement<>& stmt) {
  stmt.expr->accept(*this);
  cout << ";" << std::endl;
}

void PrintVisitor::visitBlock(Block<>& block) {
  cout << "{" << std::endl;
  for(vector<unique_ptr<Statement<> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
  cout << "}" << std::endl;
}

void PrintVisitor::visitAdditionExpression(AdditionExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "+";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitSubtractionExpression(SubtractionExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "-";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitMultiplicationExpression(MultiplicationExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "*";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitDivisionExpression(DivisionExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "/";
  expr.right->accept(*this);
  cout << ")";
}
void PrintVisitor::visitIntegerExpression(IntegerExpression<>& expr) {
  cout << expr.value;
}
void PrintVisitor::visitVariableExpression(VariableExpression<>& expr) {
  cout << expr.id;
}
