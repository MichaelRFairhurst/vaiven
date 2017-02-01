#ifndef VAIVEN_VISITOR_HEADER_PRINT_VISITOR
#define VAIVEN_VISITOR_HEADER_PRINT_VISITOR

#include "../ast/visitor.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;

class PrintVisitor : public Visitor<bool> {

  public:
  virtual void visitAdditionExpression(AdditionExpression<bool>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<bool>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<bool>& expr);
  virtual void visitDivisionExpression(DivisionExpression<bool>& expr);
  virtual void visitIntegerExpression(IntegerExpression<bool>& expr);

};

}}

#endif
