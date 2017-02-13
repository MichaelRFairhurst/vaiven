#include "location_resolver.h"

using namespace vaiven::visitor;

void LocationResolver::visitIfStatement(IfStatement<>& stmt) {
  stmt.condition->accept(*this);
  unique_ptr<Expression<Location> > condition(move(exprCopyStack.top()));
  exprCopyStack.pop();

  vector<unique_ptr<Statement<Location> > > newTrueStmts;
  vector<unique_ptr<Statement<Location> > > newFalseStmts;
  {
    ScopeFrame<bool> scopeFrame(scope);
    for(vector<unique_ptr<Statement<> > >::iterator it = stmt.trueStatements.begin();
        it != stmt.trueStatements.end();
        ++it) {
      (*it)->accept(*this);
      unique_ptr<Statement<Location> > newStmt(move(stmtCopyStack.top()));
      stmtCopyStack.pop();
      newTrueStmts.push_back(move(newStmt));
    }
  }

  {
    ScopeFrame<bool> scopeFrame(scope);
    for(vector<unique_ptr<Statement<> > >::iterator it = stmt.falseStatements.begin();
        it != stmt.falseStatements.end();
        ++it) {
      (*it)->accept(*this);
      unique_ptr<Statement<Location> > newStmt(move(stmtCopyStack.top()));
      stmtCopyStack.pop();
      newFalseStmts.push_back(move(newStmt));
    }
  }

  Location void_ = Location::void_();
  unique_ptr<Statement<Location> > copy(new IfStatement<Location>(
    move(condition),
    move(newTrueStmts),
    move(newFalseStmts)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitReturnStatement(ReturnStatement<>& stmt) {
  stmt.expr->accept(*this);
  unique_ptr<Expression<Location> > expr(move(exprCopyStack.top()));
  exprCopyStack.pop();
  Location void_ = Location::void_();
  unique_ptr<Statement<Location> > copy(new ReturnStatement<Location>(move(expr)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitVarDecl(VarDecl<>& varDecl) {
  varDecl.expr->accept(*this);
  scope.put(varDecl.varname, true);
  unique_ptr<Expression<Location> > expr(move(exprCopyStack.top()));
  exprCopyStack.pop();
  Location void_ = Location::void_();
  unique_ptr<Statement<Location> > copy(new VarDecl<Location>(varDecl.varname, move(expr)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitFuncCallExpression(FuncCallExpression<>& expr) {
  vector<unique_ptr<Expression<Location> > > newParams;
  for(vector<unique_ptr<Expression<> > >::iterator it = expr.parameters.begin();
      it != expr.parameters.end();
      ++it) {
    (*it)->accept(*this);
    unique_ptr<Expression<Location> > newParam(move(exprCopyStack.top()));
    exprCopyStack.pop();
    newParams.push_back(move(newParam));
  }

  Location void_ = Location::void_();
  unique_ptr<Expression<Location> > copy(new FuncCallExpression<Location>(expr.name, move(newParams)));
  copy->resolvedData = void_;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitFuncDecl(FuncDecl<>& decl) {
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

void LocationResolver::visitExpressionStatement(ExpressionStatement<>& stmt) {
  stmt.expr->accept(*this);
  unique_ptr<Expression<Location> > expr(move(exprCopyStack.top()));
  exprCopyStack.pop();
  Location void_ = Location::void_();
  unique_ptr<Statement<Location> > copy(new ExpressionStatement<Location>(move(expr)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitBlock(Block<>& block) {
  ScopeFrame<bool> scopeFrame(scope);
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

void LocationResolver::visitAdditionExpression(AdditionExpression<>& expr) {
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
void LocationResolver::visitSubtractionExpression(SubtractionExpression<>& expr) {
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
void LocationResolver::visitMultiplicationExpression(MultiplicationExpression<>& expr) {
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
void LocationResolver::visitDivisionExpression(DivisionExpression<>& expr) {
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
void LocationResolver::visitIntegerExpression(IntegerExpression<>& expr) {
  Location immediate = Location::imm(expr.value);
  unique_ptr<Expression<Location> > copy(new IntegerExpression<Location>(expr.value));
  copy->resolvedData = immediate;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitVariableExpression(VariableExpression<>& expr) {
  if (argIndexes.find(expr.id) == argIndexes.end()) {
    if (scope.contains(expr.id)) {
      unique_ptr<Expression<Location> > copy(new VariableExpression<Location>(expr.id));
      copy->resolvedData = Location::local();
      exprCopyStack.push(copy.release());
      return;
    }
    throw "unknown var name";
  }

  int argNum = argIndexes[expr.id];
  Location var_loc = Location::arg(argNum);

  unique_ptr<Expression<Location> > copy(new VariableExpression<Location>(expr.id));
  copy->resolvedData = var_loc;
  exprCopyStack.push(copy.release());
}
