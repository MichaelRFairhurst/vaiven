#include "ssa_builder.h"

#include "../ast/all.h"

using namespace vaiven::visitor;
using namespace vaiven::ssa;

void SsaBuilder::emit(Instruction* next) {
  if (curBlock->head == NULL) {
    curBlock->head = next;
  } else {
    writePoint->next = next;
  }
  cur = next;
  writePoint = cur;
}

void SsaBuilder::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  stmt.condition->accept(*this);
  JmpCcInstr* jmp = new JmpCcInstr(cur);
  ConditionalBlockExit* exitToTrue = new ConditionalBlockExit(jmp);
  UnconditionalBlockExit* exitToFalse = new UnconditionalBlockExit();
  UnconditionalBlockExit* trueExit = new UnconditionalBlockExit();
  UnconditionalBlockExit* falseExit = new UnconditionalBlockExit();
  
  ssa::Block* trueBlock = new ssa::Block();
  ssa::Block* falseBlock = new ssa::Block();
  ssa::Block* followBlock = new ssa::Block();

  curBlock->next = trueBlock;
  trueBlock->next = falseBlock;
  falseBlock->next = followBlock;

  exitToTrue->toGoTo = trueBlock;
  exitToFalse->toGoTo = falseBlock;
  trueExit->toGoTo = followBlock;
  falseExit->toGoTo = followBlock;

  curBlock->exits.push_back(unique_ptr<BlockExit>(exitToTrue));
  curBlock->exits.push_back(unique_ptr<BlockExit>(exitToFalse));
  trueBlock->exits.push_back(unique_ptr<BlockExit>(trueExit));
  falseBlock->exits.push_back(unique_ptr<BlockExit>(falseExit));

  curBlock = trueBlock;
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.trueStatements.begin();
      it != stmt.trueStatements.end();
      ++it) {
    (*it)->accept(*this);
  }

  curBlock = falseBlock;
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.falseStatements.begin();
      it != stmt.falseStatements.end();
      ++it) {
    (*it)->accept(*this);
  }

  curBlock = followBlock;
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
