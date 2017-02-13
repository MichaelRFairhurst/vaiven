#ifndef VAIVEN_AST_HEADER_IF_STATEMENT
#define VAIVEN_AST_HEADER_IF_STATEMENT

#include <memory>
#include <vector>
#include "statement.h"
#include "expression.h"

using std::unique_ptr;
using std::vector;

namespace vaiven { namespace ast {

template<typename RD>
class IfStatement : public Statement<RD> {

  public:
  IfStatement(
      unique_ptr<Expression<RD> > condition,
      vector<unique_ptr<Statement<RD> > > trueStatements,
      vector<unique_ptr<Statement<RD> > > falseStatements)
      : condition(std::move(condition))
      , trueStatements(std::move(trueStatements))
      , falseStatements(std::move(falseStatements)) {};

  void accept(Visitor<RD>& v) {
    return v.visitIfStatement(*this);
  }
  virtual ~IfStatement() {};

  unique_ptr<Expression<RD> > condition;
  vector<unique_ptr<Statement<RD> > > trueStatements;
  vector<unique_ptr<Statement<RD> > > falseStatements;
};

}}

#endif
