#ifndef VAIVEN_VISITOR_HEADER_AUTO_COMPILER
#define VAIVEN_VISITOR_HEADER_AUTO_COMPILER

#include <stack>
#include <vector>
#include <memory>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../type_info.h"
#include "../functions.h"
#include "../scope.h"
#include "../error_compiler.h"

#include "../asmjit/src/asmjit/asmjit.h"

namespace vaiven { namespace visitor {

using std::stack;
using std::vector;
using std::unique_ptr;
using namespace vaiven::ast;
using asmjit::X86Compiler;

class AutoCompiler : public Visitor<TypedLocationInfo> {

  public:
  AutoCompiler(X86Compiler& cc, asmjit::CodeHolder& codeHolder, Functions& funcs)
    : cc(cc), codeHolder(codeHolder), funcs(funcs), error(cc) {}

  void compile(Node<TypedLocationInfo>& expr);

  virtual void visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr);
  virtual void visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr);
  virtual void visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr);
  virtual void visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr);
  virtual void visitDoubleExpression(DoubleExpression<TypedLocationInfo>& expr);
  virtual void visitStringExpression(StringExpression<TypedLocationInfo>& expr);
  virtual void visitVariableExpression(VariableExpression<TypedLocationInfo>& expr);
  virtual void visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr);
  virtual void visitNotExpression(NotExpression<TypedLocationInfo>& expr);
  virtual void visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr);
  virtual void visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr);
  virtual void visitGtExpression(GtExpression<TypedLocationInfo>& expr);
  virtual void visitGteExpression(GteExpression<TypedLocationInfo>& expr);
  virtual void visitLtExpression(LtExpression<TypedLocationInfo>& expr);
  virtual void visitLteExpression(LteExpression<TypedLocationInfo>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr);
  virtual void visitListLiteralExpression(ListLiteralExpression<TypedLocationInfo>& expr);
  virtual void visitDynamicAccessExpression(DynamicAccessExpression<TypedLocationInfo>& expr);
  virtual void visitDynamicStoreExpression(DynamicStoreExpression<TypedLocationInfo>& expr);
  virtual void visitStaticAccessExpression(StaticAccessExpression<TypedLocationInfo>& expr);
  virtual void visitStaticStoreExpression(StaticStoreExpression<TypedLocationInfo>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt);
  virtual void visitIfStatement(IfStatement<TypedLocationInfo>& stmt);
  virtual void visitForCondition(ForCondition<TypedLocationInfo>& stmt);
  virtual void visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt);
  virtual void visitBlock(Block<TypedLocationInfo>& expr);
  virtual void visitFuncDecl(FuncDecl<TypedLocationInfo>& funcDecl);
  virtual void visitVarDecl(VarDecl<TypedLocationInfo>& varDecl);

  void doPreAssignmentOp(asmjit::X86Gp currentVal, ast::PreAssignmentOp preAssignmentOp, ast::Expression<TypedLocationInfo>& newVal);
  void doCmpIntExpression(Expression<TypedLocationInfo>& left, Expression<TypedLocationInfo>& right);
  void doCmpNotExpression(NotExpression<TypedLocationInfo>& expr);
  // returns if ended in jmp or vreg
  bool doCmpEqualityExpression(Expression<TypedLocationInfo>& left, Expression<TypedLocationInfo>& right, bool checkTrue);

  protected:
  void generateTypeShapePrelog(FuncDecl<TypedLocationInfo>& funcDecl, FunctionUsage* usage);
  void generateOptimizeProlog(FuncDecl<TypedLocationInfo>& funcDecl, asmjit::FuncSignature& sig);
  void generateTypeErrorProlog();
  void typecheckInt(asmjit::X86Gp vreg, TypedLocationInfo& info);
  void typecheckBool(asmjit::X86Gp vreg, TypedLocationInfo& info);
  void box(asmjit::X86Gp vreg, TypedLocationInfo& info);
  void box(asmjit::X86Gp vreg, VaivenStaticType type);

  X86Compiler& cc;
  asmjit::CodeHolder& codeHolder;
  Functions& funcs;
  stack<asmjit::X86Gp> vRegs;
  vector<asmjit::X86Gp> argRegs;
  string curFuncName;
  asmjit::CCFunc* curFunc;
  asmjit::Label optimizeLabel;
  Scope<asmjit::X86Gp> scope;

  ErrorCompiler error;

};

}}

#endif
