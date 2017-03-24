#ifndef VAIVEN_AST_HEADER_DYNAMIC_STORE_EXPRESSION
#define VAIVEN_AST_HEADER_DYNAMIC_STORE_EXPRESSION

#include <memory>
#include <string>
#include "expression.h"
#include "pre_assignment_op.h"

using std::unique_ptr;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class DynamicStoreExpression : public Expression<RD> {

  public:
  DynamicStoreExpression(
      unique_ptr<Expression<RD> > subject,
      unique_ptr<Expression<RD> > property,
      unique_ptr<Expression<RD> > rhs,
      PreAssignmentOp preAssignmentOp)
    : property(std::move(property)), subject(std::move(subject)), rhs(std::move(rhs)), preAssignmentOp(preAssignmentOp) {};

  void accept(Visitor<RD>& v) {
    return v.visitDynamicStoreExpression(*this);
  }
  virtual ~DynamicStoreExpression() {};

  unique_ptr<Expression<RD> > subject;
  unique_ptr<Expression<RD> > property;
  unique_ptr<Expression<RD> > rhs;
  PreAssignmentOp preAssignmentOp;
};

}}

#endif
