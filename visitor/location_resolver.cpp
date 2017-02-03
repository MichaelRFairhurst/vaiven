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
  Location immediate = Location::imm(expr.value);
  unique_ptr<Expression<Location> > copy(new IntegerExpression<Location>(expr.value));
  copy->resolvedData = immediate;
  copyStack.push(copy.release());
}
void LocationResolver::visitVariableExpression(VariableExpression<bool>& expr) {
  if (argIndexes.find(expr.id) == argIndexes.end()) {
    int count = argIndexes.size(); // safer to break these up, I think
    argIndexes[expr.id] = count; // C++ modifyX = modifyX is undefined?
  }

  int argNum = argIndexes[expr.id];
  Location var_loc = Location::arg(argNum);

  unique_ptr<Expression<Location> > copy(new VariableExpression<Location>(expr.id));
  copy->resolvedData = var_loc;
  copyStack.push(copy.release());
}
