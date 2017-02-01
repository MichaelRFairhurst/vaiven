#ifndef VAIVEN_AST_HEADER_INTEGER_EXPRESSION
#define VAIVEN_AST_HEADER_INTEGER_EXPRESSION

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename ResolvedData>
class IntegerExpression : public Expression<ResolvedData> {

  public:
  IntegerExpression(int value) : value(value) {};

  void accept(Visitor<ResolvedData>& v) {
    v.visitIntegerExpression(*this);
  }
  virtual ~IntegerExpression() {};

  int value;
};

}}

#endif
