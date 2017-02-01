#include "location_resolver.h"

using namespace vaiven::visitor;

void LocationResolver::visitAdditionExpression(AdditionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(copyStack.top()));
  copyStack.pop();
  Location loc = lhs->resolvedData;
  if (loc.type == LOCATION_TYPE_IMM) {
    loc = Location(); // spill
  }
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
  Location loc = lhs->resolvedData;
  if (loc.type == LOCATION_TYPE_IMM) {
    loc = Location(); // spill
  }
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
  Location loc = lhs->resolvedData;
  if (loc.type == LOCATION_TYPE_IMM) {
    loc = Location(); // spill
  }
  unique_ptr<Expression<Location> > copy(new MultiplicationExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  copyStack.push(copy.release());
}
void LocationResolver::visitDivisionExpression(DivisionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  Location rax((asmjit::X86Gp*) &asmjit::x86::rax);
  unique_ptr<Expression<Location> > rhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(copyStack.top()));
  copyStack.pop();
  unique_ptr<Expression<Location> > copy(new DivisionExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = rax;
  copyStack.push(copy.release());
}
void LocationResolver::visitIntegerExpression(IntegerExpression<bool>& expr) {
  Location immediate(expr.value);
  unique_ptr<Expression<Location> > copy(new IntegerExpression<Location>(expr.value));
  copy->resolvedData = immediate;
  copyStack.push(copy.release());
}
