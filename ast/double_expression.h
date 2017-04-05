#ifndef VAIVEN_AST_HEADER_DOUBLE_EXPRESSION
#define VAIVEN_AST_HEADER_DOUBLE_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class DoubleExpression : public Expression<RD> {

  public:
  DoubleExpression(double value) : value(value) {};

  void accept(Visitor<RD>& v) {
    return v.visitDoubleExpression(*this);
  }
  virtual ~DoubleExpression() {};

  double value;
};

}}

#endif
