#include "print_visitor.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

void PrintVisitor::visitFuncDecl(FuncDecl<bool>& decl) {
  cout << "fn " << decl.name << " of ";
  for (vector<string>::iterator it = decl.args.begin();
      it != decl.args.end();
      ++it) {
    cout << *it << ", ";
  }
  cout << "is" << std::endl;
  for(vector<unique_ptr<Statement<bool> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
  cout << std::endl << "end" << std::endl;
}

void PrintVisitor::visitExpressionStatement(ExpressionStatement<bool>& stmt) {
  stmt.expr->accept(*this);
  cout << ";" << std::endl;
}

void PrintVisitor::visitBlock(Block<bool>& block) {
  cout << "{" << std::endl;
  for(vector<unique_ptr<Statement<bool> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
  cout << "}" << std::endl;
}

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
void PrintVisitor::visitVariableExpression(VariableExpression<bool>& expr) {
  cout << expr.id;
}
