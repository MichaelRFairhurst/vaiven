#ifndef VAIVEN_VISITOR_HEADER_SSA_BUILDER
#define VAIVEN_VISITOR_HEADER_SSA_BUILDER

#include <stack>
#include <map>
#include <string>
#include <unordered_set>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../type_info.h"
#include "../scope.h"
#include "../ssa/ssa.h"
#include "../ssa/cfg.h"
#include "../function_usage.h"
#include "../functions.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;
using std::stack;
using std::map;
using std::string;
using std::unordered_set;

class SsaBuilder : public Visitor<> {

  public:
  SsaBuilder(FunctionUsage& usageInfo, Functions& funcs)
      : usageInfo(usageInfo), funcs(funcs), curBlock(&head), cur(NULL), writePoint(NULL) {};

  virtual void visitAssignmentExpression(AssignmentExpression<>& expr);
  virtual void visitAdditionExpression(AdditionExpression<>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<>& expr);
  virtual void visitDivisionExpression(DivisionExpression<>& expr);
  virtual void visitIntegerExpression(IntegerExpression<>& expr);
  virtual void visitDoubleExpression(DoubleExpression<>& expr);
  virtual void visitStringExpression(StringExpression<>& expr);
  virtual void visitVariableExpression(VariableExpression<>& expr);
  virtual void visitBoolLiteral(BoolLiteral<>& expr);
  virtual void visitNotExpression(NotExpression<>& expr);
  virtual void visitEqualityExpression(EqualityExpression<>& expr);
  virtual void visitInequalityExpression(InequalityExpression<>& expr);
  virtual void visitGtExpression(GtExpression<>& expr);
  virtual void visitGteExpression(GteExpression<>& expr);
  virtual void visitLtExpression(LtExpression<>& expr);
  virtual void visitLteExpression(LteExpression<>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<>& expr);
  virtual void visitListLiteralExpression(ListLiteralExpression<>& expr);
  virtual void visitDynamicAccessExpression(DynamicAccessExpression<>& expr);
  virtual void visitDynamicStoreExpression(DynamicStoreExpression<>& expr);
  virtual void visitStaticAccessExpression(StaticAccessExpression<>& expr);
  virtual void visitStaticStoreExpression(StaticStoreExpression<>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<>& stmt);
  virtual void visitIfStatement(IfStatement<>& stmt);
  virtual void visitForCondition(ForCondition<>& stmt);
  virtual void visitReturnStatement(ReturnStatement<>& stmt);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);
  virtual void visitVarDecl(VarDecl<>& varDecl);

  void emit(ssa::Instruction* next);

  ssa::Block head;
  ssa::Block* curBlock;
  ssa::Instruction* cur;
  ssa::Instruction* writePoint;
  Scope<ssa::Instruction*> scope;

  private:
  Functions& funcs;
  FunctionUsage& usageInfo;
  unordered_set<string> varsToPhi;
  bool isReturnable;

};

}}

#endif
