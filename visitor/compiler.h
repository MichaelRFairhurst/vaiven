#ifndef VAIVEN_VISITOR_HEADER_COMPILER
#define VAIVEN_VISITOR_HEADER_COMPILER

#include <stack>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../type_info.h"
#include "../functions.h"

#include "../asmjit/src/asmjit/asmjit.h"

#define asm assembler

namespace vaiven { namespace visitor {

using std::stack;
using namespace vaiven::ast;
using asmjit::X86Assembler;

class Compiler : public Visitor<TypedLocationInfo> {

  public:
  Compiler(X86Assembler& asm, asmjit::CodeHolder& codeHolder, Functions& funcs)
      : asm(asm), funcs(funcs), codeHolder(codeHolder) {};

  void compile(Node<TypedLocationInfo>& root);

  virtual void visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr);
  virtual void visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr);
  virtual void visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr);
  virtual void visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr);
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
  virtual void visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt);
  virtual void visitIfStatement(IfStatement<TypedLocationInfo>& stmt);
  virtual void visitForCondition(ForCondition<TypedLocationInfo>& stmt);
  virtual void visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt);
  virtual void visitBlock(Block<TypedLocationInfo>& expr);
  virtual void visitFuncDecl(FuncDecl<TypedLocationInfo>& funcDecl);
  virtual void visitVarDecl(VarDecl<TypedLocationInfo>& varDecl);

  private:
  X86Assembler& asm;
  asmjit::CodeHolder& codeHolder;
  Functions& funcs;
};

}}

#endif
