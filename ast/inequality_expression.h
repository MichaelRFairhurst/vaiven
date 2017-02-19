#ifndef VAIVEN_AST_HEADER_INEQUALITY_EXPRESSION
#define VAIVEN_AST_HEADER_INEQUALITY_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class InequalityExpression : public Expression<RD> {

  public:
  InequalityExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitInequalityExpression(*this);
  }
  virtual ~InequalityExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
