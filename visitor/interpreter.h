#ifndef VAIVEN_VISITOR_HEADER_INTERPRETER
#define VAIVEN_VISITOR_HEADER_INTERPRETER

#include <stack>

#include "../ast/visitor.h"

namespace vaiven { namespace visitor {

using std::stack;
using namespace vaiven::ast;

class Interpreter : public Visitor<bool> {

  public:
  virtual void visitAdditionExpression(AdditionExpression<bool>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<bool>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<bool>& expr);
  virtual void visitDivisionExpression(DivisionExpression<bool>& expr);
  virtual void visitIntegerExpression(IntegerExpression<bool>& expr);

  stack<int> stack;
};

}}

#endif
