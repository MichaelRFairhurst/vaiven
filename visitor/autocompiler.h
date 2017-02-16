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

#include "../asmjit/src/asmjit/asmjit.h"

namespace vaiven { namespace visitor {

using std::stack;
using std::vector;
using std::unique_ptr;
using namespace vaiven::ast;
using asmjit::X86Compiler;

class AutoCompiler : public Visitor<TypedLocationInfo> {

  public:
  AutoCompiler(X86Compiler& cc, asmjit::CodeHolder& codeHolder, Functions& funcs) : cc(cc), codeHolder(codeHolder), funcs(funcs) {};

  void compile(Node<TypedLocationInfo>& expr, int numVars);

  virtual void visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr);
  virtual void visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr);
  virtual void visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr);
  virtual void visitVariableExpression(VariableExpression<TypedLocationInfo>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt);
  virtual void visitIfStatement(IfStatement<TypedLocationInfo>& stmt);
  virtual void visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt);
  virtual void visitBlock(Block<TypedLocationInfo>& expr);
  virtual void visitFuncDecl(FuncDecl<TypedLocationInfo>& funcDecl);
  virtual void visitVarDecl(VarDecl<TypedLocationInfo>& varDecl);


  private:
  void typecheckInt(asmjit::X86Gp vreg, TypedLocationInfo& info);
  void box(asmjit::X86Gp vreg, TypedLocationInfo& info);

  X86Compiler& cc;
  asmjit::CodeHolder& codeHolder;
  Functions& funcs;
  stack<asmjit::X86Gp> vRegs;
  vector<asmjit::X86Gp> argRegs;
  string curFuncName;
  asmjit::CCFunc* curFunc;
  asmjit::Label typeErrorLabel;
  Scope<asmjit::X86Gp> scope;

};

}}

#endif
