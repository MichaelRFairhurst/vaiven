#ifndef VAIVEN_AST_HEADER_LTE_EXPRESSION
#define VAIVEN_AST_HEADER_LTE_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class LteExpression : public Expression<RD> {

  public:
  LteExpression(
      unique_ptr<Expression<RD> > left,
      unique_ptr<Expression<RD> > right)
      : left(std::move(left)), right(std::move(right)) {};

  void accept(Visitor<RD>& v) {
    return v.visitLteExpression(*this);
  }
  virtual ~LteExpression() {};

  unique_ptr<Expression<RD> > left;
  unique_ptr<Expression<RD> > right;
};

}}

#endif
