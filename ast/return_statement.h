#ifndef VAIVEN_AST_HEADER_RETURN_STATEMENT
#define VAIVEN_AST_HEADER_RETURN_STATEMENT

#include <memory>
#include "statement.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class ReturnStatement : public Statement<RD> {

  public:
  ReturnStatement(
      unique_ptr<Expression<RD> > expr)
      : expr(std::move(expr)) {};

  void accept(Visitor<RD>& v) {
    return v.visitReturnStatement(*this);
  }
  virtual ~ReturnStatement() {};

  unique_ptr<Expression<RD> > expr;
};

}}

#endif
