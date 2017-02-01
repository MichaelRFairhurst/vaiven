#ifndef VAIVEN_AST_HEADER_SUBTRACTION_EXPRESSION
#define VAIVEN_AST_HEADER_SUBTRACTION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename ResolvedData=void>
class SubtractionExpression : public Expression<ResolvedData> {

  public:
  SubtractionExpression(
      unique_ptr<Expression<ResolvedData> > left,
      unique_ptr<Expression<ResolvedData> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<ResolvedData>& v) {
    v.visitSubtractionExpression(*this);
  }
  virtual ~SubtractionExpression() {};

  unique_ptr<Expression<ResolvedData> > left;
  unique_ptr<Expression<ResolvedData> > right;
};

}}

#endif
