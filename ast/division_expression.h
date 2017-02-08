#ifndef VAIVEN_AST_HEADER_DIVISION_EXPRESSION
#define VAIVEN_AST_HEADER_DIVISION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class DivisionExpression : public Expression<RD> {

  public:
  DivisionExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitDivisionExpression(*this);
  }
  virtual ~DivisionExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
