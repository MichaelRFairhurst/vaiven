#ifndef VAIVEN_AST_HEADER_EQUALITY_EXPRESSION
#define VAIVEN_AST_HEADER_EQUALITY_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class EqualityExpression : public Expression<RD> {

  public:
  EqualityExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitEqualityExpression(*this);
  }
  virtual ~EqualityExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
