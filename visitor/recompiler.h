#ifndef VAIVEN_VISITOR_HEADER_RECOMPILER
#define VAIVEN_VISITOR_HEADER_RECOMPILER

#include <stack>
#include <vector>
#include <memory>

#include "autocompiler.h"

namespace vaiven { namespace visitor {

class ReCompiler : public AutoCompiler {

  public:
  ReCompiler(X86Compiler& cc, asmjit::CodeHolder& codeHolder, Functions& funcs, FunctionUsage& usageInfo)
    : AutoCompiler(cc, codeHolder, funcs), usageInfo(usageInfo) {};

  //void compile(Node<TypedLocationInfo>& expr);

  //virtual void visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr);
  //virtual void visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr);
  //virtual void visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr);
  //virtual void visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr);
  //virtual void visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr);
  //virtual void visitVariableExpression(VariableExpression<TypedLocationInfo>& expr);
  //virtual void visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr);
  //virtual void visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt);
  //virtual void visitIfStatement(IfStatement<TypedLocationInfo>& stmt);
  //virtual void visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt);
  //virtual void visitBlock(Block<TypedLocationInfo>& expr);
  virtual void visitFuncDecl(FuncDecl<TypedLocationInfo>& funcDecl);
  //virtual void visitVarDecl(VarDecl<TypedLocationInfo>& varDecl);


  private:

  FunctionUsage& usageInfo;

};

}}

#endif
