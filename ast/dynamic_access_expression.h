#ifndef VAIVEN_AST_HEADER_DYNAMIC_ACCESS_EXPRESSION
#define VAIVEN_AST_HEADER_DYNAMIC_ACCESS_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class DynamicAccessExpression : public Expression<RD> {

  public:
  DynamicAccessExpression(
      unique_ptr<Expression<RD> > subject,
      unique_ptr<Expression<RD> > property)
      : subject(std::move(subject)), property(std::move(property)) {};

  void accept(Visitor<RD>& v) {
    return v.visitDynamicAccessExpression(*this);
  }
  virtual ~DynamicAccessExpression() {};

  unique_ptr<Expression<RD> > subject;
  unique_ptr<Expression<RD> > property;
};

}}

#endif
