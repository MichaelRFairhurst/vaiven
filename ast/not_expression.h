#ifndef VAIVEN_AST_HEADER_NOT_EXPRESSION
#define VAIVEN_AST_HEADER_NOT_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class NotExpression : public Expression<RD> {

  public:
  NotExpression(unique_ptr<Expression<RD> > expr) : expr(std::move(expr)) {};

  void accept(Visitor<RD>& v) {
    return v.visitNotExpression(*this);
  }
  virtual ~NotExpression() {};

  unique_ptr<Expression<RD> > expr;
};

}}

#endif
