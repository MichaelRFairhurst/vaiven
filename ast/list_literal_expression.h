#ifndef VAIVEN_AST_HEADER_LIST_LITERAL_EXPRESSION
#define VAIVEN_AST_HEADER_LIST_LITERAL_EXPRESSION

#include <string>
#include <memory>
#include <vector>
#include "expression.h"

using std::unique_ptr;
using std::vector;
using std::string;

namespace vaiven { namespace ast {

template<typename RD>
class ListLiteralExpression : public Expression<RD> {

  public:
  ListLiteralExpression(vector<unique_ptr<Expression<RD> > > items) :
      items(std::move(items)) {};

  void accept(Visitor<RD>& v) {
    return v.visitListLiteralExpression(*this);
  }
  virtual ~ListLiteralExpression() {};

  vector<unique_ptr<Expression<RD> > > items;
};

}}

#endif
