#ifndef VAIVEN_AST_HEADER_STATIC_STORE_EXPRESSION
#define VAIVEN_AST_HEADER_STATIC_STORE_EXPRESSION

#include <memory>
#include <string>
#include "expression.h"

using std::unique_ptr;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class StaticStoreExpression : public Expression<RD> {

  public:
  StaticStoreExpression(
      unique_ptr<Expression<RD> > subject,
      string property,
      unique_ptr<Expression<RD> > rhs)
      : subject(std::move(subject)), property(property), rhs(std::move(rhs)) {};

  void accept(Visitor<RD>& v) {
    return v.visitStaticStoreExpression(*this);
  }
  virtual ~StaticStoreExpression() {};

  unique_ptr<Expression<RD> > subject;
  string property;
  unique_ptr<Expression<RD> > rhs;
};

}}

#endif
