#ifndef VAIVEN_AST_HEADER_FUNC_CALL_EXPRESSION
#define VAIVEN_AST_HEADER_FUNC_CALL_EXPRESSION

#include <string>
#include <memory>
#include <vector>
#include "expression.h"

using std::unique_ptr;
using std::vector;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class FuncCallExpression : public Expression<RD> {

  public:
  FuncCallExpression(string name, vector<unique_ptr<Expression<RD> > > parameters) :
      name(name),
      parameters(std::move(parameters)) {};

  void accept(Visitor<RD>& v) {
    return v.visitFuncCallExpression(*this);
  }
  virtual ~FuncCallExpression() {};

  vector<unique_ptr<Expression<RD> > > parameters;
  string name;
};

}}

#endif
