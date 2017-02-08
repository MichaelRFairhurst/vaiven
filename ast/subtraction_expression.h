#ifndef VAIVEN_AST_HEADER_SUBTRACTION_EXPRESSION
#define VAIVEN_AST_HEADER_SUBTRACTION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class SubtractionExpression : public Expression<RD> {

  public:
  SubtractionExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitSubtractionExpression(*this);
  }
  virtual ~SubtractionExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
