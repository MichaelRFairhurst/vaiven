#ifndef VAIVEN_AST_HEADER_VAR_DECL
#define VAIVEN_AST_HEADER_VAR_DECL

#include <memory>
#include <string>
#include "statement.h"

using std::unique_ptr;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class VarDecl : public Statement<RD> {

  public:
  VarDecl(
      string varname, unique_ptr<Expression<RD> > expr)
      : varname(varname), expr(std::move(expr)) {};

  void accept(Visitor<RD>& v) {
    return v.visitVarDecl(*this);
  }
  virtual ~VarDecl() {};

  unique_ptr<Expression<RD> > expr;
  string varname;
};

}}

#endif
