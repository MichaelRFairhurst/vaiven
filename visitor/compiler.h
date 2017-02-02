#ifndef VAIVEN_VISITOR_HEADER_COMPILER
#define VAIVEN_VISITOR_HEADER_COMPILER

#include <stack>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../location.h"

#include "../asmjit/src/asmjit/asmjit.h"

#define asm assembler

namespace vaiven { namespace visitor {

using std::stack;
using namespace vaiven::ast;
using asmjit::X86Assembler;

class Compiler : public Visitor<Location> {

  public:
  Compiler(X86Assembler& asm) : asm(asm) {};

  void compile(Expression<Location>& root);

  virtual void visitAdditionExpression(AdditionExpression<Location>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<Location>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<Location>& expr);
  virtual void visitDivisionExpression(DivisionExpression<Location>& expr);
  virtual void visitIntegerExpression(IntegerExpression<Location>& expr);
  virtual void visitVariableExpression(VariableExpression<Location>& expr);

  private:
  X86Assembler& asm;
};

}}

#endif