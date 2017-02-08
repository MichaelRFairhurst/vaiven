#ifndef VAIVEN_AST_HEADER_MULTIPLICATION_EXPRESSION
#define VAIVEN_AST_HEADER_MULTIPLICATION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class MultiplicationExpression : public Expression<RD> {

  public:
  MultiplicationExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitMultiplicationExpression(*this);
  }
  virtual ~MultiplicationExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
