#ifndef VAIVEN_AST_HEADER_ADDITION_EXPRESSION
#define VAIVEN_AST_HEADER_ADDITION_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD=bool>
class AdditionExpression : public Expression<RD> {

  public:
  AdditionExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitAdditionExpression(*this);
  }
  virtual ~AdditionExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
