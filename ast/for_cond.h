#ifndef VAIVEN_AST_HEADER_FOR_COND
#define VAIVEN_AST_HEADER_FOR_COND

#include <memory>
#include <vector>
#include "statement.h"
#include "expression.h"

using std::unique_ptr;
using std::vector;

namespace vaiven { namespace ast {

template<typename RD>
class ForCondition : public Statement<RD> {

  public:
  ForCondition(
      unique_ptr<Expression<RD> > condition,
      vector<unique_ptr<Statement<RD> > > statements)
      : condition(std::move(condition))
      , statements(std::move(statements)) {};

  void accept(Visitor<RD>& v) {
    return v.visitForCondition(*this);
  }
  virtual ~ForCondition() {};

  unique_ptr<Expression<RD> > condition;
  vector<unique_ptr<Statement<RD> > > statements;
};

}}

#endif
