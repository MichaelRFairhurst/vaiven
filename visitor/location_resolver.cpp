#include "location_resolver.h"

using namespace vaiven::visitor;

void LocationResolver::visitIfStatement(IfStatement<>& stmt) {
  stmt.condition->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > condition(move(exprCopyStack.top()));
  exprCopyStack.pop();

  vector<unique_ptr<Statement<TypedLocationInfo> > > newTrueStmts;
  vector<unique_ptr<Statement<TypedLocationInfo> > > newFalseStmts;
  {
    ScopeFrame<bool> scopeFrame(scope);
    for(vector<unique_ptr<Statement<> > >::iterator it = stmt.trueStatements.begin();
        it != stmt.trueStatements.end();
        ++it) {
      (*it)->accept(*this);
      unique_ptr<Statement<TypedLocationInfo> > newStmt(move(stmtCopyStack.top()));
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
      unique_ptr<Statement<TypedLocationInfo> > newStmt(move(stmtCopyStack.top()));
      stmtCopyStack.pop();
      newFalseStmts.push_back(move(newStmt));
    }
  }

  TypedLocationInfo void_(Location::void_(), VAIVEN_STATIC_TYPE_VOID, false);
  unique_ptr<Statement<TypedLocationInfo> > copy(new IfStatement<TypedLocationInfo>(
    move(condition),
    move(newTrueStmts),
    move(newFalseStmts)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitForCondition(ForCondition<>& stmt) {
  stmt.condition->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > condition(move(exprCopyStack.top()));
  exprCopyStack.pop();

  vector<unique_ptr<Statement<TypedLocationInfo> > > newStmts;
  {
    ScopeFrame<bool> scopeFrame(scope);
    for(vector<unique_ptr<Statement<> > >::iterator it = stmt.statements.begin();
        it != stmt.statements.end();
        ++it) {
      (*it)->accept(*this);
      unique_ptr<Statement<TypedLocationInfo> > newStmt(move(stmtCopyStack.top()));
      stmtCopyStack.pop();
      newStmts.push_back(move(newStmt));
    }
  }

  TypedLocationInfo void_(Location::void_(), VAIVEN_STATIC_TYPE_VOID, false);
  unique_ptr<Statement<TypedLocationInfo> > copy(new ForCondition<TypedLocationInfo>(
    move(condition),
    move(newStmts)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitReturnStatement(ReturnStatement<>& stmt) {
  stmt.expr->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > expr(move(exprCopyStack.top()));
  exprCopyStack.pop();
  TypedLocationInfo void_(Location::void_(), VAIVEN_STATIC_TYPE_VOID, false);
  unique_ptr<Statement<TypedLocationInfo> > copy(new ReturnStatement<TypedLocationInfo>(move(expr)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitVarDecl(VarDecl<>& varDecl) {
  varDecl.expr->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > expr(move(exprCopyStack.top()));
  exprCopyStack.pop();
  TypedLocationInfo loc;
  if (scope.contains(varDecl.varname) || argIndexes.find(varDecl.varname) != argIndexes.end()) {
    loc = TypedLocationInfo(Location::arg(-1), VAIVEN_STATIC_TYPE_VOID, false);
  } else {
    loc = TypedLocationInfo(Location::spilled(), VAIVEN_STATIC_TYPE_VOID, false);
    scope.put(varDecl.varname, true);
  }
  unique_ptr<Statement<TypedLocationInfo> > copy(new VarDecl<TypedLocationInfo>(varDecl.varname, move(expr)));
  copy->resolvedData = loc;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitFuncCallExpression(FuncCallExpression<>& expr) {
  vector<unique_ptr<Expression<TypedLocationInfo> > > newParams;
  for(vector<unique_ptr<Expression<> > >::iterator it = expr.parameters.begin();
      it != expr.parameters.end();
      ++it) {
    (*it)->accept(*this);
    unique_ptr<Expression<TypedLocationInfo> > newParam(move(exprCopyStack.top()));
    exprCopyStack.pop();
    newParams.push_back(move(newParam));
  }

  TypedLocationInfo void_(Location::void_(), VAIVEN_STATIC_TYPE_UNKNOWN, true);
  unique_ptr<Expression<TypedLocationInfo> > copy(new FuncCallExpression<TypedLocationInfo>(expr.name, move(newParams)));
  copy->resolvedData = void_;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitListLiteralExpression(ListLiteralExpression<>& expr) {
  vector<unique_ptr<Expression<TypedLocationInfo> > > newItems;
  for(vector<unique_ptr<Expression<> > >::iterator it = expr.items.begin();
      it != expr.items.end();
      ++it) {
    (*it)->accept(*this);
    unique_ptr<Expression<TypedLocationInfo> > newItem(move(exprCopyStack.top()));
    exprCopyStack.pop();
    newItems.push_back(move(newItem));
  }

  TypedLocationInfo void_(Location::void_(), VAIVEN_STATIC_TYPE_LIST, true);
  unique_ptr<Expression<TypedLocationInfo> > copy(new ListLiteralExpression<TypedLocationInfo>(move(newItems)));
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

  vector<unique_ptr<Statement<TypedLocationInfo> > > newStmts;
  for(vector<unique_ptr<Statement<> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
    unique_ptr<Statement<TypedLocationInfo> > newStmt(move(stmtCopyStack.top()));
    stmtCopyStack.pop();
    newStmts.push_back(move(newStmt));
  }

  TypedLocationInfo void_(Location::void_(), VAIVEN_STATIC_TYPE_VOID, false);
  unique_ptr<FuncDecl<TypedLocationInfo> > copy(new FuncDecl<TypedLocationInfo>(decl.name, decl.args, move(newStmts)));
  copy->resolvedData = void_;
  nodeCopyStack.push(copy.release());
}

void LocationResolver::visitExpressionStatement(ExpressionStatement<>& stmt) {
  stmt.expr->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > expr(move(exprCopyStack.top()));
  exprCopyStack.pop();
  TypedLocationInfo typeinfo = expr->resolvedData;
  unique_ptr<Statement<TypedLocationInfo> > copy(new ExpressionStatement<TypedLocationInfo>(move(expr)));
  copy->resolvedData = typeinfo;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitBlock(Block<>& block) {
  ScopeFrame<bool> scopeFrame(scope);
  vector<unique_ptr<Statement<TypedLocationInfo> > > newStmts;
  for(vector<unique_ptr<Statement<> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
    unique_ptr<Statement<TypedLocationInfo> > newStmt(move(stmtCopyStack.top()));
    stmtCopyStack.pop();
    newStmts.push_back(move(newStmt));
  }

  TypedLocationInfo void_(Location::void_(), VAIVEN_STATIC_TYPE_VOID, false);
  unique_ptr<Statement<TypedLocationInfo> > copy(new Block<TypedLocationInfo>(move(newStmts)));
  copy->resolvedData = void_;
  stmtCopyStack.push(copy.release());
}

void LocationResolver::visitAssignmentExpression(AssignmentExpression<>& expr) {
  TypedLocationInfo loc;
  if (argIndexes.find(expr.varname) == argIndexes.end()) {
    if (scope.contains(expr.varname)) {
      loc = TypedLocationInfo(Location::local(), VAIVEN_STATIC_TYPE_UNKNOWN, true);
    } else {
      loc = TypedLocationInfo(Location::arg(-1), VAIVEN_STATIC_TYPE_UNKNOWN, true);
    }
  } else {
    int argNum = argIndexes[expr.varname];
    loc = TypedLocationInfo(Location::arg(argNum), VAIVEN_STATIC_TYPE_UNKNOWN, true);
  }

  expr.expr->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > inner(move(exprCopyStack.top()));
  exprCopyStack.pop();
  loc.type = inner->resolvedData.type;
  unique_ptr<Expression<TypedLocationInfo> > copy(new AssignmentExpression<TypedLocationInfo>(expr.varname, move(inner)));
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitAdditionExpression(AdditionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  TypedLocationInfo loc;
  if (lhs->resolvedData.type == VAIVEN_STATIC_TYPE_INT
      && rhs->resolvedData.type == VAIVEN_STATIC_TYPE_INT) {
    loc = TypedLocationInfo(Location::spilled(), VAIVEN_STATIC_TYPE_INT, false);
  } else if (lhs->resolvedData.type == VAIVEN_STATIC_TYPE_STRING
      && rhs->resolvedData.type == VAIVEN_STATIC_TYPE_STRING) {
    loc = TypedLocationInfo(Location::spilled(), VAIVEN_STATIC_TYPE_STRING, true);
  } else {
    loc = TypedLocationInfo(Location::spilled(), VAIVEN_STATIC_TYPE_UNKNOWN, true);
  }
  unique_ptr<Expression<TypedLocationInfo> > copy(new AdditionExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitSubtractionExpression(SubtractionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_INT, false);
  unique_ptr<Expression<TypedLocationInfo> > copy(new SubtractionExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitMultiplicationExpression(MultiplicationExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_INT, false);
  unique_ptr<Expression<TypedLocationInfo> > copy(new MultiplicationExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitDivisionExpression(DivisionExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new DivisionExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_INT, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitIntegerExpression(IntegerExpression<>& expr) {
  TypedLocationInfo immediate(Location::imm(expr.value), VAIVEN_STATIC_TYPE_INT, true);
  unique_ptr<Expression<TypedLocationInfo> > copy(new IntegerExpression<TypedLocationInfo>(expr.value));
  copy->resolvedData = immediate;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitStringExpression(StringExpression<>& expr) {
  TypedLocationInfo immediate(Location::imm((uint64_t) &expr.value), VAIVEN_STATIC_TYPE_STRING, true);
  // copy the string because at destruct it starts to be GCed
  unique_ptr<Expression<TypedLocationInfo> > copy(new StringExpression<TypedLocationInfo>(new GcableString(expr.value->str)));
  copy->resolvedData = immediate;
  exprCopyStack.push(copy.release());
}
void LocationResolver::visitVariableExpression(VariableExpression<>& expr) {
  if (argIndexes.find(expr.id) == argIndexes.end()) {
    unique_ptr<Expression<TypedLocationInfo> > copy(new VariableExpression<TypedLocationInfo>(expr.id));
    copy->resolvedData = TypedLocationInfo(Location::local(), VAIVEN_STATIC_TYPE_UNKNOWN, true);
    if (scope.contains(expr.id)) {
      copy->resolvedData = TypedLocationInfo(Location::local(), VAIVEN_STATIC_TYPE_UNKNOWN, true);
    } else {
      copy->resolvedData = TypedLocationInfo(Location::arg(-1), VAIVEN_STATIC_TYPE_UNKNOWN, true);
    }
    exprCopyStack.push(copy.release());
    return;
  }

  int argNum = argIndexes[expr.id];
  TypedLocationInfo var_loc(Location::arg(argNum), VAIVEN_STATIC_TYPE_UNKNOWN, true);

  unique_ptr<Expression<TypedLocationInfo> > copy(new VariableExpression<TypedLocationInfo>(expr.id));
  copy->resolvedData = var_loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitBoolLiteral(BoolLiteral<>& expr) {
  TypedLocationInfo immediate(Location::imm(expr.value), VAIVEN_STATIC_TYPE_BOOL, true);
  unique_ptr<Expression<TypedLocationInfo> > copy(new BoolLiteral<TypedLocationInfo>(expr.value));
  copy->resolvedData = immediate;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitNotExpression(NotExpression<>& expr) {
  expr.expr->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > innerCopy(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new NotExpression<TypedLocationInfo>(move(innerCopy)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_BOOL, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitInequalityExpression(InequalityExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new InequalityExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_BOOL, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitEqualityExpression(EqualityExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new EqualityExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_BOOL, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitGtExpression(GtExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new GtExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_BOOL, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitGteExpression(GteExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new GteExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_BOOL, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitLtExpression(LtExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new LtExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_BOOL, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}

void LocationResolver::visitLteExpression(LteExpression<>& expr) {
  expr.left->accept(*this);
  expr.right->accept(*this);
  unique_ptr<Expression<TypedLocationInfo> > rhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > lhs(move(exprCopyStack.top()));
  exprCopyStack.pop();
  unique_ptr<Expression<TypedLocationInfo> > copy(new LteExpression<TypedLocationInfo>(move(lhs), move(rhs)));
  TypedLocationInfo loc(Location::spilled(), VAIVEN_STATIC_TYPE_BOOL, false);
  copy->resolvedData = loc;
  exprCopyStack.push(copy.release());
}
