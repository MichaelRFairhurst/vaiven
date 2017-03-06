#include "ssa_builder.h"

#include "../ast/all.h"

using namespace vaiven::visitor;
using namespace vaiven::ssa;

void SsaBuilder::emit(Instruction* next) {
  if (curBlock->head.get() == NULL) {
    curBlock->head.reset(next);
  } else {
    writePoint->next = next;
  }
  cur = next;
  writePoint = cur;
}

void SsaBuilder::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  stmt.condition->accept(*this);
  emit(new NotInstr(cur));
  JmpCcInstr* jmp = new JmpCcInstr(cur);
  
  ssa::Block* trueBlock = new ssa::Block();
  curBlock->next.reset(trueBlock);
  ssa::Block* falseBlock = new ssa::Block();
  trueBlock->next.reset(falseBlock);
  ssa::Block* followBlock = new ssa::Block();
  falseBlock->next.reset(followBlock);

  ConditionalBlockExit* jmpToFalse = new ConditionalBlockExit(jmp, falseBlock);
  curBlock->exits.push_back(unique_ptr<BlockExit>(jmpToFalse));
  UnconditionalBlockExit* jmpToTrue = new UnconditionalBlockExit(trueBlock);
  curBlock->exits.push_back(unique_ptr<BlockExit>(jmpToTrue));
  UnconditionalBlockExit* jmpFromTrue = new UnconditionalBlockExit(followBlock);
  trueBlock->exits.push_back(unique_ptr<BlockExit>(jmpFromTrue));
  UnconditionalBlockExit* jmpFromFalse = new UnconditionalBlockExit(followBlock);
  falseBlock->exits.push_back(unique_ptr<BlockExit>(jmpFromFalse));

  unordered_set<string> prevVarsToPhi = varsToPhi;
  map<string, Instruction*> scopeStatePreIf;
  map<string, Instruction*> modifiedTrue;
  scope.fill(scopeStatePreIf);

  {
    ScopeFrame<Instruction*> initScope(scope);
    curBlock = trueBlock;
    for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.trueStatements.begin();
        it != stmt.trueStatements.end();
        ++it) {
      (*it)->accept(*this);
    }

    for (unordered_set<string>::iterator it = varsToPhi.begin(); it != varsToPhi.end(); ++it) {
      modifiedTrue[*it] = scope.get(*it);
      scope.replace(*it, scopeStatePreIf[*it]);
    }
  }

  prevVarsToPhi.insert(varsToPhi.begin(), varsToPhi.end());
  varsToPhi.clear();

  {
    ScopeFrame<Instruction*> initScope(scope);
    curBlock = falseBlock;
    for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.falseStatements.begin();
        it != stmt.falseStatements.end();
        ++it) {
      (*it)->accept(*this);
    }

    // phis have to be in the follow block
    curBlock = followBlock;

    for (map<string, Instruction*>::iterator it = modifiedTrue.begin(); it != modifiedTrue.end(); ++it) {
      if (varsToPhi.find(it->first) == varsToPhi.end()) {
        // phi(before_if, in_true)
        Instruction* newPhi = new PhiInstr(it->second, scopeStatePreIf[it->first]);
        emit(newPhi);
        scope.replace(it->first, newPhi);

        // erase so that all varsToPhi left over are not in modifiedTrue for sure
        varsToPhi.erase(it->first);
      } else {
        // phi(in_false, in_true)
        Instruction* newPhi = new PhiInstr(it->second, scope.get(it->first));
        emit(newPhi);
        scope.replace(it->first, newPhi);
      }
    }

    // all varsToPhi left over are not in modifiedTrue
    for (unordered_set<string>::iterator it = varsToPhi.begin(); it != varsToPhi.end(); ++it) {
      // phi(before_if, in_false)
      Instruction* newPhi = new PhiInstr(scopeStatePreIf[*it], scope.get(*it));
      emit(newPhi);
      scope.replace(*it, newPhi);
    }
  }

  // restore previous varsToPhi, some are from this scope which doesn't affect upper ifs
  varsToPhi.insert(prevVarsToPhi.begin(), prevVarsToPhi.end());
  unordered_set<string>::iterator it = varsToPhi.begin();
  while (it != varsToPhi.end()) {
    if (!scope.inHigherScope(*it)) {
      it = varsToPhi.erase(it);
    } else {
      ++it;
    }
  }
}

void SsaBuilder::visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
  Instruction* retVal = cur;
  emit(new RetInstr(retVal));
}

void SsaBuilder::visitVarDecl(VarDecl<TypedLocationInfo>& varDecl) {
  varDecl.expr->accept(*this);
  scope.put(varDecl.varname, cur);
}

void SsaBuilder::visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr) {
  CallInstr* call = new CallInstr(expr.name);

  for (int i = 0; i < expr.parameters.size(); ++i) {
    expr.parameters[i]->accept(*this);
    call->inputs.push_back(cur);
    cur->usages.insert(call);
  }

  emit(call);
}

void SsaBuilder::visitFuncDecl(FuncDecl<TypedLocationInfo>& decl) {
  for (int i = 0; i < decl.args.size(); ++i) {
    ArgInstr* arg = new ArgInstr(i, usageInfo.argShapes[i].getStaticType());
    emit(arg); // part of the instruction stream for ownership purposes
    scope.put(decl.args[i], arg);
  }

  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
  }

  if (cur != NULL) {
    emit(new RetInstr(cur));
  }
}

void SsaBuilder::visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
}

void SsaBuilder::visitBlock(Block<TypedLocationInfo>& block) {
  throw "not supported";
}

void SsaBuilder::visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr) {
  expr.expr->accept(*this);
  if (!scope.contains(expr.varname)) {
    emit(new ErrInstr());
    return;
  }

  // x = y; if foo do x = z; ... use phi after block
  if (scope.inHigherScope(expr.varname)) {
    varsToPhi.insert(expr.varname);
  }

  scope.replace(expr.varname, cur);
}

void SsaBuilder::visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new AddInstr(lhs, rhs));
}

void SsaBuilder::visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new SubInstr(lhs, rhs));
}

void SsaBuilder::visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new MulInstr(lhs, rhs));
}

void SsaBuilder::visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new DivInstr(lhs, rhs));
}

void SsaBuilder::visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr) {
  emit(new ConstantInstr(Value(expr.value)));
}

void SsaBuilder::visitVariableExpression(VariableExpression<TypedLocationInfo>& expr) {
  if (scope.contains(expr.id)) {
    // don't emit the instruction, just place it as the cur so other instructions
    // can reference it
    cur = scope.get(expr.id);
  } else {
    emit(new ErrInstr());
  }
}

void SsaBuilder::visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr) {
  emit(new ConstantInstr(Value(expr.value)));
}

void SsaBuilder::visitNotExpression(NotExpression<TypedLocationInfo>& expr) {
  expr.expr->accept(*this);
  Instruction* inner = cur;
  emit(new NotInstr(inner));
}

void SsaBuilder::visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpIneqInstr(lhs, rhs));
}

void SsaBuilder::visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpEqInstr(lhs, rhs));
}

void SsaBuilder::visitGtExpression(GtExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpGtInstr(lhs, rhs));
}

void SsaBuilder::visitGteExpression(GteExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpGteInstr(lhs, rhs));
}

void SsaBuilder::visitLtExpression(LtExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpLtInstr(lhs, rhs));
}

void SsaBuilder::visitLteExpression(LteExpression<TypedLocationInfo>& expr) {
  expr.left->accept(*this);
  Instruction* lhs = cur;
  expr.right->accept(*this);
  Instruction* rhs = cur;
  emit(new CmpLteInstr(lhs, rhs));
}
