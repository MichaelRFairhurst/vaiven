#ifndef VAIVEN_AST_HEADER_INTEGER_EXPRESSION
#define VAIVEN_AST_HEADER_INTEGER_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class IntegerExpression : public Expression<RD> {

  public:
  IntegerExpression(int value) : value(value) {};

  void accept(Visitor<RD>& v) {
    return v.visitIntegerExpression(*this);
  }
  virtual ~IntegerExpression() {};

  int value;
};

}}

#endif
