#include "autocompiler.h"

#include "../ast/all.h"

using namespace asmjit;
using namespace vaiven::visitor;

void AutoCompiler::compile(Expression<Location>& root) {
  cc.addFunc(FuncSignature0<int, int, int, int, int, int, int>());
  root.accept(*this);
  cc.ret(vRegs.top());
  cc.endFunc();
  cc.finalize();
}

void AutoCompiler::visitAdditionExpression(AdditionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newGpd();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.add(result, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, rhsReg);
    cc.add(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.add(result, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.add(result, rhsReg);
  }
  vRegs.push(result);
}

void AutoCompiler::visitSubtractionExpression(SubtractionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newGpd();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.sub(result, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, rhsReg);
    cc.neg(result);
    cc.add(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.sub(result, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.sub(result, rhsReg);
  }
  vRegs.push(result);
}
void AutoCompiler::visitMultiplicationExpression(MultiplicationExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newGpd();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.imul(result, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, rhsReg);
    cc.imul(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.imul(result, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.imul(result, rhsReg);
  }
  vRegs.push(result);
}
void AutoCompiler::visitDivisionExpression(DivisionExpression<Location>& expr) {
  Location& left_loc = expr.left->resolvedData;
  Location& right_loc = expr.right->resolvedData;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newGpd();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result, left_loc.data.imm);
    cc.add(result, right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, rhsReg);
    cc.add(result, left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.add(result, right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.mov(result, lhsReg);
    cc.add(result, rhsReg);
  }
  vRegs.push(result);
}

void AutoCompiler::visitIntegerExpression(IntegerExpression<Location>& expr) {
  // should only happen when in a stmt by itself
  X86Gp var = cc.newGpd();
  cc.mov(var, expr.value);
  vRegs.push(var);
}

void AutoCompiler::visitVariableExpression(VariableExpression<Location>& expr) {
  X86Gp var = cc.newGpd();
  cc.mov(var, 1000);
  vRegs.push(var);
}
