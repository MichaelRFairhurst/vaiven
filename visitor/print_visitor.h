#ifndef VAIVEN_VISITOR_HEADER_PRINT_VISITOR
#define VAIVEN_VISITOR_HEADER_PRINT_VISITOR

#include "../ast/visitor.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;

class PrintVisitor : public Visitor<> {

  public:
  virtual void visitAssignmentExpression(AssignmentExpression<>& expr);
  virtual void visitAdditionExpression(AdditionExpression<>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<>& expr);
  virtual void visitDivisionExpression(DivisionExpression<>& expr);
  virtual void visitIntegerExpression(IntegerExpression<>& expr);
  virtual void visitVariableExpression(VariableExpression<>& expr);
  virtual void visitBoolLiteral(BoolLiteral<>& expr);
  virtual void visitNotExpression(NotExpression<>& expr);
  virtual void visitEqualityExpression(EqualityExpression<>& expr);
  virtual void visitInequalityExpression(InequalityExpression<>& expr);
  virtual void visitGtExpression(GtExpression<>& expr);
  virtual void visitGteExpression(GteExpression<>& expr);
  virtual void visitLtExpression(LtExpression<>& expr);
  virtual void visitLteExpression(LteExpression<>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<>& stmt);
  virtual void visitIfStatement(IfStatement<>& stmt);
  virtual void visitForCondition(ForCondition<>& stmt);
  virtual void visitReturnStatement(ReturnStatement<>& stmt);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);
  virtual void visitVarDecl(VarDecl<>& varDecl);


};

}}

#endif
