#include "location_resolver.h"

using namespace vaiven::visitor;

void LocationResolver::visitAdditionExpression(AdditionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(copyStack.top()));
  copyStack.pop();
  Location loc = Location(); // spill
  unique_ptr<Expression<Location> > copy(new AdditionExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  copyStack.push(copy.release());
}
void LocationResolver::visitSubtractionExpression(SubtractionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(copyStack.top()));
  copyStack.pop();
  Location loc = Location(); // spill
  unique_ptr<Expression<Location> > copy(new SubtractionExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  copyStack.push(copy.release());
}
void LocationResolver::visitMultiplicationExpression(MultiplicationExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(copyStack.top()));
  copyStack.pop();
  Location loc = Location(); // spill
  unique_ptr<Expression<Location> > copy(new MultiplicationExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  copyStack.push(copy.release());
}
void LocationResolver::visitDivisionExpression(DivisionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > copy(new DivisionExpression<Location>(move(lhs), move(rhs)));
  Location loc = Location(); // spill
  copy->resolvedData = loc;
  copyStack.push(copy.release());
}
void LocationResolver::visitIntegerExpression(IntegerExpression<bool>& expr) {
  Location immediate(expr.value);
  unique_ptr<Expression<Location> > copy(new IntegerExpression<Location>(expr.value));
  copy->resolvedData = immediate;
  copyStack.push(copy.release());
}
void LocationResolver::visitVariableExpression(VariableExpression<bool>& expr) {
  if (argIndexes.find(expr.id) == argIndexes.end()) {
    int count = argIndexes.size() + 1; // safer to break these up, I think
    argIndexes[expr.id] = count; // C++ modifyX = modifyX is undefined?
  }

  Location var_loc(1000);
  int argNum = argIndexes[expr.id];
  switch (argNum) {
    case 1:
      var_loc = Location((asmjit::X86Gp*) &asmjit::x86::rdi); break;
    case 2:
      var_loc = Location((asmjit::X86Gp*) &asmjit::x86::rsi); break;
    case 3:
      var_loc = Location((asmjit::X86Gp*) &asmjit::x86::rdx); break;
    case 4:
      var_loc = Location((asmjit::X86Gp*) &asmjit::x86::rcx); break;
    case 5:
      var_loc = Location((asmjit::X86Gp*) &asmjit::x86::r8); break;
    case 6:
      var_loc = Location((asmjit::X86Gp*) &asmjit::x86::r9); break;
  }

  unique_ptr<Expression<Location> > copy(new VariableExpression<Location>(expr.id));
  copy->resolvedData = var_loc;
  copyStack.push(copy.release());
}
