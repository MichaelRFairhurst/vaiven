#ifndef VAIVEN_VISITOR_HEADER_AUTO_COMPILER
#define VAIVEN_VISITOR_HEADER_AUTO_COMPILER

#include <stack>
#include <vector>
#include <memory>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../location.h"

#include "../asmjit/src/asmjit/asmjit.h"

namespace vaiven { namespace visitor {

using std::stack;
using std::vector;
using std::unique_ptr;
using namespace vaiven::ast;
using asmjit::X86Compiler;

class AutoCompiler : public Visitor<Location> {

  public:
  AutoCompiler(X86Compiler& cc) : cc(cc) {};

  void compile(Node<Location>& expr, int numVars);

  virtual void visitAdditionExpression(AdditionExpression<Location>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<Location>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<Location>& expr);
  virtual void visitDivisionExpression(DivisionExpression<Location>& expr);
  virtual void visitIntegerExpression(IntegerExpression<Location>& expr);
  virtual void visitVariableExpression(VariableExpression<Location>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<Location>& expr);
  virtual void visitBlock(Block<Location>& expr);
  virtual void visitFuncDecl(FuncDecl<Location>& funcDecl);

  private:
  X86Compiler& cc;
  stack<asmjit::X86Gp> vRegs;
  vector<asmjit::X86Gp> argRegs;

};

}}

#endif
