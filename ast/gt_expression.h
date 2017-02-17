#ifndef VAIVEN_AST_HEADER_GT_EXPRESSION
#define VAIVEN_AST_HEADER_GT_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class GtExpression : public Expression<RD> {

  public:
  GtExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitGtExpression(*this);
  }
  virtual ~GtExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
