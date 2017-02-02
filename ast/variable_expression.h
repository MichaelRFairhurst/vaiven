#ifndef VAIVEN_AST_HEADER_VARIABLE_EXPRESSION
#define VAIVEN_AST_HEADER_VARIABLE_EXPRESSION

#include <string>
#include "expression.h"

using std::string;

namespace vaiven { namespace ast {

template<typename ResolvedData=void>
class VariableExpression : public Expression<ResolvedData> {

  public:
  VariableExpression(string id) : id(id) {};

  void accept(Visitor<ResolvedData>& v) {
    v.visitVariableExpression(*this);
  }
  virtual ~VariableExpression() {};

  string id;
};

}}

#endif
