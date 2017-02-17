#ifndef VAIVEN_VISITOR_HEADER_LOCATION_RESOLVER
#define VAIVEN_VISITOR_HEADER_LOCATION_RESOLVER

#include <map>
#include <memory>
#include <stack>
#include <string>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../type_info.h"
#include "../scope.h"

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
  virtual void visitBoolLiteral(BoolLiteral<>& expr);
  virtual void visitEqualityExpression(EqualityExpression<>& expr);
  virtual void visitGtExpression(GtExpression<>& expr);
  virtual void visitGteExpression(GteExpression<>& expr);
  virtual void visitLtExpression(LtExpression<>& expr);
  virtual void visitLteExpression(LteExpression<>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<>& stmt);
  virtual void visitIfStatement(IfStatement<>& stmt);
  virtual void visitReturnStatement(ReturnStatement<>& stmt);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);
  virtual void visitVarDecl(VarDecl<>& varDecl);

  stack<Expression<TypedLocationInfo>* > exprCopyStack;
  stack<Statement<TypedLocationInfo>* > stmtCopyStack;
  stack<Node<TypedLocationInfo>* > nodeCopyStack;
  map<string, int> argIndexes;

  Scope<bool> scope;

};

}}

#endif
