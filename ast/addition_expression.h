#ifndef VAIVEN_AST_HEADER_ADDITION_EXPRESSION
#define VAIVEN_AST_HEADER_ADDITION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename ResolvedData=void>
class AdditionExpression : public Expression<ResolvedData> {

  public:
  AdditionExpression(
      unique_ptr<Expression<ResolvedData> > left,
      unique_ptr<Expression<ResolvedData> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<ResolvedData>& v) {
  v.visitAdditionExpression(*this);
}
  virtual ~AdditionExpression() {};

  unique_ptr<Expression<ResolvedData> > left;
  unique_ptr<Expression<ResolvedData> > right;
};

}}

#endif
