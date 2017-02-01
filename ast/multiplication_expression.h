#ifndef VAIVEN_AST_HEADER_MULTIPLICATION_EXPRESSION
#define VAIVEN_AST_HEADER_MULTIPLICATION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename ResolvedData=void>
class MultiplicationExpression : public Expression<ResolvedData> {

  public:
  MultiplicationExpression(
      unique_ptr<Expression<ResolvedData> > left,
      unique_ptr<Expression<ResolvedData> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<ResolvedData>& v) {
    v.visitMultiplicationExpression(*this);
  }
  virtual ~MultiplicationExpression() {};

  unique_ptr<Expression<ResolvedData> > left;
  unique_ptr<Expression<ResolvedData> > right;
};

}}

#endif
