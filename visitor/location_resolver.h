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

class LocationResolver : public Visitor<> {

  public:
  virtual void visitAdditionExpression(AdditionExpression<>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<>& expr);
  virtual void visitDivisionExpression(DivisionExpression<>& expr);
  virtual void visitIntegerExpression(IntegerExpression<>& expr);
  virtual void visitVariableExpression(VariableExpression<>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<>& expr);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);

  stack<Expression<Location>* > exprCopyStack;
  stack<Statement<Location>* > stmtCopyStack;
  stack<Node<Location>* > nodeCopyStack;
  map<string, int> argIndexes;

};

}}

#endif
