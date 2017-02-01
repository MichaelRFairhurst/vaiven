#ifndef VAIVEN_AST_HEADER_DIVISION_EXPRESSION
#define VAIVEN_AST_HEADER_DIVISION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename ResolvedData=void>
class DivisionExpression : public Expression<ResolvedData> {

  public:
  DivisionExpression(
      unique_ptr<Expression<ResolvedData> > left,
      unique_ptr<Expression<ResolvedData> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<ResolvedData>& v) {
    v.visitDivisionExpression(*this);
  }
  virtual ~DivisionExpression() {};

  unique_ptr<Expression<ResolvedData> > left;
  unique_ptr<Expression<ResolvedData> > right;
};

}}

#endif
