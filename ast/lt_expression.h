#ifndef VAIVEN_AST_HEADER_LT_EXPRESSION
#define VAIVEN_AST_HEADER_LT_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class LtExpression : public Expression<RD> {

  public:
  LtExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitLtExpression(*this);
  }
  virtual ~LtExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
