#ifndef VAIVEN_VISITOR_HEADER_PRINT_VISITOR
#define VAIVEN_VISITOR_HEADER_PRINT_VISITOR

#include "../ast/visitor.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;

class PrintVisitor : public Visitor<> {

  public:
  virtual void visitAdditionExpression(AdditionExpression<>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<>& expr);
  virtual void visitDivisionExpression(DivisionExpression<>& expr);
  virtual void visitIntegerExpression(IntegerExpression<>& expr);
  virtual void visitVariableExpression(VariableExpression<>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<>& expr);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);

};

}}

#endif
