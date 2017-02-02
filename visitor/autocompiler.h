#ifndef VAIVEN_VISITOR_HEADER_AUTO_COMPILER
#define VAIVEN_VISITOR_HEADER_AUTO_COMPILER

#include <stack>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../location.h"

#include "../asmjit/src/asmjit/asmjit.h"

namespace vaiven { namespace visitor {

using std::stack;
using namespace vaiven::ast;
using asmjit::X86Compiler;

class AutoCompiler : public Visitor<Location> {

  public:
  AutoCompiler(X86Compiler& cc) : cc(cc) {};

  void compile(Expression<Location>& expr);

  virtual void visitAdditionExpression(AdditionExpression<Location>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<Location>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<Location>& expr);
  virtual void visitDivisionExpression(DivisionExpression<Location>& expr);
  virtual void visitIntegerExpression(IntegerExpression<Location>& expr);
  virtual void visitVariableExpression(VariableExpression<Location>& expr);

  private:
  X86Compiler& cc;
  stack<asmjit::X86Gp> vRegs;

};

}}

#endif
