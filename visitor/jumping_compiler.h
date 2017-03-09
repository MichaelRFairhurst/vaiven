#ifndef VAIVEN_VISITOR_HEADER_JUMPING_COMPILER
#define VAIVEN_VISITOR_HEADER_JUMPING_COMPILER

#include <stack>
#include <vector>
#include <memory>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../type_info.h"
#include "autocompiler.h"

#include "../asmjit/src/asmjit/asmjit.h"

namespace vaiven { namespace visitor {

using namespace vaiven::ast;
using asmjit::X86Compiler;

class JumpingCompiler : public Visitor<TypedLocationInfo> {

  public:
  JumpingCompiler(
      X86Compiler& cc,
      AutoCompiler& compiler,
      asmjit::Label label,
      bool jmpFalse)
    : cc(cc), compiler(compiler), label(label), jmpFalse(jmpFalse), didJmp(false) {};

  virtual void visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr);
  virtual void visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr);
  virtual void visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr);
  virtual void visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr);
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

  bool didJmp;

  protected:
  X86Compiler& cc;
  AutoCompiler& compiler;
  asmjit::Label label;
  bool jmpFalse;

};

}}

#endif
