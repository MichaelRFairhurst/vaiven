#ifndef VAIVEN_AST_HEADER_GTE_EXPRESSION
#define VAIVEN_AST_HEADER_GTE_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class GteExpression : public Expression<RD> {

  public:
  GteExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitGteExpression(*this);
  }
  virtual ~GteExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
