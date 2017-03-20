#ifndef VAIVEN_AST_HEADER_DYNAMIC_STORE_EXPRESSION
#define VAIVEN_AST_HEADER_DYNAMIC_STORE_EXPRESSION

#include <memory>
#include <string>
#include "expression.h"

using std::unique_ptr;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class DynamicStoreExpression : public Expression<RD> {

  public:
  DynamicStoreExpression(
      unique_ptr<Expression<RD> > subject,
      unique_ptr<Expression<RD> > property,
      unique_ptr<Expression<RD> > rhs)
    : property(std::move(property)), subject(std::move(subject)), rhs(std::move(rhs)) {};

  void accept(Visitor<RD>& v) {
    return v.visitDynamicStoreExpression(*this);
  }
  virtual ~DynamicStoreExpression() {};

  unique_ptr<Expression<RD> > subject;
  unique_ptr<Expression<RD> > property;
  unique_ptr<Expression<RD> > rhs;
};

}}

#endif
