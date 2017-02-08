#ifndef VAIVEN_AST_HEADER_EXPRESSION_STATEMENT
#define VAIVEN_AST_HEADER_EXPRESSION_STATEMENT

#include <memory>
#include "statement.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD=bool>
class ExpressionStatement : public Statement<RD> {

  public:
  ExpressionStatement(
      unique_ptr<Expression<RD> > expr)
      : expr(std::move(expr)) {};

  void accept(Visitor<RD>& v) {
    return v.visitExpressionStatement(*this);
  }
  virtual ~ExpressionStatement() {};

  unique_ptr<Expression<RD> > expr;
};

}}

#endif
