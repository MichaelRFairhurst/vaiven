#ifndef VAIVEN_VISITOR_HEADER_LOCATION_RESOLVER
#define VAIVEN_VISITOR_HEADER_LOCATION_RESOLVER

#include <map>
#include <memory>
#include <stack>
#include <string>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../location.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;
using namespace vaiven;
using namespace std;

class LocationResolver : public Visitor<bool> {

  public:
  virtual void visitAdditionExpression(AdditionExpression<bool>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<bool>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<bool>& expr);
  virtual void visitDivisionExpression(DivisionExpression<bool>& expr);
  virtual void visitIntegerExpression(IntegerExpression<bool>& expr);
  virtual void visitVariableExpression(VariableExpression<bool>& expr);

  stack<Expression<Location>* > copyStack;
  map<string, int> argIndexes;

};

}}

#endif
