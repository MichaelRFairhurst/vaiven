#ifndef VAIVEN_AST_HEADER_STATIC_ACCESS_EXPRESSION
#define VAIVEN_AST_HEADER_STATIC_ACCESS_EXPRESSION

#include <memory>
#include <string>
#include "expression.h"

using std::unique_ptr;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class StaticAccessExpression : public Expression<RD> {

  public:
  StaticAccessExpression(
      unique_ptr<Expression<RD> > subject,
      string property)
      : subject(std::move(subject)), property(property) {};

  void accept(Visitor<RD>& v) {
    return v.visitStaticAccessExpression(*this);
  }
  virtual ~StaticAccessExpression() {};

  unique_ptr<Expression<RD> > subject;
  string property;
};

}}

#endif
