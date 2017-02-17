#ifndef VAIVEN_VISITOR_HEADER_USAGE_APPLIER
#define VAIVEN_VISITOR_HEADER_USAGE_APPLIER

#include <map>
#include <memory>
#include <stack>
#include <string>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../type_info.h"
#include "../scope.h"
#include "../function_usage.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;
using namespace vaiven;
using namespace std;

class UsageApplier : public Visitor<TypedLocationInfo> {

  public:
  UsageApplier(FunctionUsage& usageInfo) : usageInfo(usageInfo) {};

  virtual void visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr);
  virtual void visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr);
  virtual void visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr);
  virtual void visitVariableExpression(VariableExpression<TypedLocationInfo>& expr);
  virtual void visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr);
  virtual void visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr);
  virtual void visitGtExpression(GtExpression<TypedLocationInfo>& expr);
  virtual void visitGteExpression(GteExpression<TypedLocationInfo>& expr);
  virtual void visitLtExpression(LtExpression<TypedLocationInfo>& expr);
  virtual void visitLteExpression(LteExpression<TypedLocationInfo>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt);
  virtual void visitIfStatement(IfStatement<TypedLocationInfo>& stmt);
  virtual void visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt);
  virtual void visitBlock(Block<TypedLocationInfo>& expr);
  virtual void visitFuncDecl(FuncDecl<TypedLocationInfo>& funcDecl);
  virtual void visitVarDecl(VarDecl<TypedLocationInfo>& varDecl);

  map<string, int> argIndexes;

  FunctionUsage& usageInfo;

};

}}

#endif
