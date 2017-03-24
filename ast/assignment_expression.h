#ifndef VAIVEN_AST_HEADER_ASSIGNMENT_EXPRESSION
#define VAIVEN_AST_HEADER_ASSIGNMENT_EXPRESSION

#include <memory>
#include <string>
#include "expression.h"
#include "pre_assignment_op.h"

using std::unique_ptr;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class AssignmentExpression : public Expression<RD> {

  public:
  AssignmentExpression(
      string varname, unique_ptr<Expression<RD> > expr,
      PreAssignmentOp preAssignmentOp)
      : varname(varname), expr(std::move(expr)), preAssignmentOp(preAssignmentOp) {};

  void accept(Visitor<RD>& v) {
    return v.visitAssignmentExpression(*this);
  }
  virtual ~AssignmentExpression() {};

  string varname;
  unique_ptr<Expression<RD> > expr;
  PreAssignmentOp preAssignmentOp;
};

}}

#endif
