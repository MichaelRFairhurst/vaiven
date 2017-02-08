#ifndef VAIVEN_AST_HEADER_VARIABLE_EXPRESSION
#define VAIVEN_AST_HEADER_VARIABLE_EXPRESSION

#include <string>
#include "expression.h"

using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class VariableExpression : public Expression<RD> {

  public:
  VariableExpression(string id) : id(id) {};

  void accept(Visitor<RD>& v) {
    return v.visitVariableExpression(*this);
  }
  virtual ~VariableExpression() {};

  string id;
};

}}

#endif
