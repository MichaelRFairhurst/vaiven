#include "location_resolver.h"

using namespace vaiven::visitor;

void LocationResolver::visitFuncDecl(FuncDecl<bool>& decl) {
  int i = 0;
  for (vector<string>::iterator it = decl.args.begin();
      it != decl.args.end();
      ++it) {
    argIndexes[*it] = i++;
  }

  vector<unique_ptr<Statement<Location> > > newStmts;
  for(vector<unique_ptr<Statement<> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
    unique_ptr<Statement<Location> > newStmt(move(stmtCopyStack.top()));
    stmtCopyStack.pop();
    newStmts.push_back(move(newStmt));
  }

  Location void_ = Location::void_();
  unique_ptr<FuncDecl<Location> > copy(new FuncDecl<Location>(decl.name, decl.args, move(newStmts)));
  copy->resolvedData = void_;
  nodeCopyStack.push(copy.release());
}

void LocationResolver::visitExpressionStatement(ExpressionStatement<bool>& stmt) {
  stmt.expr->accept(*this);
  unique_ptr<Expression<Location> > expr(move(exprCopyStack.top()));
  exprCopyStack.pop();
  Location void_ = Location::void_();
  unique_ptr<Statement<Location> > copy(new ExpressionStatement<Location>(move(expr)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitBlock(Block<bool>& block) {
  vector<unique_ptr<Statement<Location> > > newStmts;
  for(vector<unique_ptr<Statement<> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
    unique_ptr<Statement<Location> > newStmt(move(stmtCopyStack.top()));
    stmtCopyStack.pop();
    newStmts.push_back(move(newStmt));
  }

  Location void_ = Location::void_();
  unique_ptr<Statement<Location> > copy(new Block<Location>(move(newStmts)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitAdditionExpression(AdditionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  Location loc = Location::spilled();
  unique_ptr<Expression<Location> > copy(new AdditionExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitSubtractionExpression(SubtractionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  Location loc = Location::spilled();
  unique_ptr<Expression<Location> > copy(new SubtractionExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitMultiplicationExpression(MultiplicationExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  Location loc = Location::spilled();
  unique_ptr<Expression<Location> > copy(new MultiplicationExpression<Location>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitDivisionExpression(DivisionExpression<bool>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<Location> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<Location> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<Location> > copy(new DivisionExpression<Location>(move(lhs), move(rhs)));
  Location loc = Location::spilled();
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitIntegerExpression(IntegerExpression<bool>& expr) {
  Location immediate = Location::imm(expr.value);
  unique_ptr<Expression<Location> > copy(new IntegerExpression<Location>(expr.value));
  copy->resolvedData = immediate;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitVariableExpression(VariableExpression<bool>& expr) {
  if (argIndexes.find(expr.id) == argIndexes.end()) {
    throw "unknown arg value";
  }

  int argNum = argIndexes[expr.id];
  Location var_loc = Location::arg(argNum);

  unique_ptr<Expression<Location> > copy(new VariableExpression<Location>(expr.id));
  copy->resolvedData = var_loc;
  exprCopyStack.push(copy.release());
}
