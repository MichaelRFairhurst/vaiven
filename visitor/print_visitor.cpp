#include "print_visitor.h"

#include <iostream>

#include "../ast/all.h"

using std::cout;

using namespace vaiven::visitor;

void PrintVisitor::visitIfStatement(IfStatement<>& stmt) {
  cout << "if ";
  stmt.condition->accept(*this);
  cout << std::endl;

  for(vector<unique_ptr<Statement<> > >::iterator it = stmt.trueStatements.begin();
      it != stmt.trueStatements.end();
      ++it) {
    (*it)->accept(*this);
  }

  cout << " else " << std::endl;

  for(vector<unique_ptr<Statement<> > >::iterator it = stmt.falseStatements.begin();
      it != stmt.falseStatements.end();
      ++it) {
    (*it)->accept(*this);
  }
 
  cout << "end" << std::endl;
}

void PrintVisitor::visitForCondition(ForCondition<>& stmt) {
  cout << "for ";
  stmt.condition->accept(*this);
  cout << std::endl;
  cout << "do " << std::endl;

  for(vector<unique_ptr<Statement<> > >::iterator it = stmt.statements.begin();
      it != stmt.statements.end();
      ++it) {
    (*it)->accept(*this);
  }

  cout << "end " << std::endl;
}

void PrintVisitor::visitReturnStatement(ReturnStatement<>& stmt) {
  cout << "return ";
  stmt.expr->accept(*this);
  cout << ";" << std::endl;
}

void PrintVisitor::visitVarDecl(VarDecl<>& varDecl) {
  cout << "var " << varDecl.varname << " = ";
  varDecl.expr->accept(*this);
  cout << ";" << std::endl;
}

void PrintVisitor::visitFuncCallExpression(FuncCallExpression<>& expr) {
  cout << expr.name << "(";
  for(vector<unique_ptr<Expression<> > >::iterator it = expr.parameters.begin();
      it != expr.parameters.end();
      ++it) {
    (*it)->accept(*this);
    cout << ", ";
  }
  cout << ")";
}

void PrintVisitor::visitListLiteralExpression(ListLiteralExpression<>& expr) {
  cout << "[";
  for(vector<unique_ptr<Expression<> > >::iterator it = expr.items.begin();
      it != expr.items.end();
      ++it) {
    (*it)->accept(*this);
    cout << ", ";
  }
  cout << "]";
}

void PrintVisitor::visitDynamicAccessExpression(DynamicAccessExpression<>& expr) {
  expr.subject->accept(*this);
  cout << "[";
  expr.property->accept(*this);
  cout << "]";
}

void PrintVisitor::visitDynamicStoreExpression(DynamicStoreExpression<>& expr) {
  expr.subject->accept(*this);
  cout << "[";
  expr.property->accept(*this);
  cout << "] = ";
  expr.rhs->accept(*this);
}

void PrintVisitor::visitStaticAccessExpression(StaticAccessExpression<>& expr) {
  expr.subject->accept(*this);
  cout << "." << expr.property;
}

void PrintVisitor::visitStaticStoreExpression(StaticStoreExpression<>& expr) {
  expr.subject->accept(*this);
  cout << "." << expr.property << " = ";
  expr.rhs->accept(*this);
}

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

void PrintVisitor::visitAssignmentExpression(AssignmentExpression<>& expr) {
  cout << expr.varname << " = ";
  expr.expr->accept(*this);
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

void PrintVisitor::visitDoubleExpression(DoubleExpression<>& expr) {
  cout << expr.value;
}

void PrintVisitor::visitStringExpression(StringExpression<>& expr) {
  cout << expr.value->str;
}

void PrintVisitor::visitVariableExpression(VariableExpression<>& expr) {
  cout << expr.id;
}

void PrintVisitor::visitBoolLiteral(BoolLiteral<>& expr) {
  cout << expr.value;
}

void PrintVisitor::visitVoidLiteral(VoidLiteral<>& expr) {
  cout << "void";
}

void PrintVisitor::visitNotExpression(NotExpression<>& expr) {
  cout << "!";
  expr.expr->accept(*this);
}

void PrintVisitor::visitInequalityExpression(InequalityExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "!=";
  expr.right->accept(*this);
  cout << ")";
}

void PrintVisitor::visitEqualityExpression(EqualityExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "==";
  expr.right->accept(*this);
  cout << ")";
}

void PrintVisitor::visitGtExpression(GtExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << ">";
  expr.right->accept(*this);
  cout << ")";
}

void PrintVisitor::visitGteExpression(GteExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << ">=";
  expr.right->accept(*this);
  cout << ")";
}

void PrintVisitor::visitLtExpression(LtExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "<";
  expr.right->accept(*this);
  cout << ")";
}

void PrintVisitor::visitLteExpression(LteExpression<>& expr) {
  cout << "(";
  expr.left->accept(*this);
  cout << "<=";
  expr.right->accept(*this);
  cout << ")";
}
