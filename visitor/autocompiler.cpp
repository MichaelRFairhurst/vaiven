#include "autocompiler.h"

#include "../ast/all.h"
#include "../value.h"
#include "../runtime_error.h"

#include <stdint.h>

using namespace asmjit;
using namespace vaiven::visitor;

void AutoCompiler::compile(Node<TypedLocationInfo>& root, int numVars) {
  root.accept(*this);
}

void AutoCompiler::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  stmt.condition->accept(*this);
  Label lfalse = cc.newLabel();
  Label lafter = cc.newLabel();
  cc.cmp(vRegs.top().r32(), 0);
  cc.je(lfalse);
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.trueStatements.begin();
      it != stmt.trueStatements.end();
      ++it) {
    (*it)->accept(*this);
  }
  cc.jmp(lafter);
  cc.bind(lfalse);
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.falseStatements.begin();
      it != stmt.falseStatements.end();
      ++it) {
    (*it)->accept(*this);
  }
  cc.bind(lafter);
}

void AutoCompiler::visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
  box(vRegs.top(), stmt.expr->resolvedData);
  cc.ret(vRegs.top());
}

void AutoCompiler::visitVarDecl(VarDecl<TypedLocationInfo>& varDecl) {
  X86Gp varReg = cc.newInt64();

  scope.put(varDecl.varname, varReg);

  varDecl.expr->accept(*this);

  box(varReg, varDecl.expr->resolvedData);
  cc.mov(varReg, vRegs.top());
  vRegs.pop();
}

void AutoCompiler::visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr) {
  CCFuncCall* funcCall;
  if (expr.name != curFuncName && funcs.funcs.find(expr.name) == funcs.funcs.end()) {
    throw "func not known";
  }
  // TODO check arg counts

  uint8_t sigArgs[expr.parameters.size()];
  vector<X86Gp> paramRegs;

  X86Gp retReg = cc.newInt64();
  for (int i = 0; i < expr.parameters.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
    expr.parameters[i]->accept(*this);
    paramRegs.push_back(vRegs.top());
    box(vRegs.top(), expr.parameters[i]->resolvedData);
    if (expr.parameters[i]->resolvedData.location.type == LOCATION_TYPE_SPILLED) {
      retReg = vRegs.top();
    }
    vRegs.pop();
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, expr.parameters.size());

  CCFuncCall* call;
  if (expr.name == curFuncName) {
    call = cc.call(curFunc->getLabel(), sig);
  } else {
    call = cc.call((unsigned long long) funcs.funcs[expr.name]->fptr, sig);
  }

  for (int i = 0; i < expr.parameters.size(); ++i) {
    call->setArg(i, paramRegs[i]);
  }

  vRegs.push(retReg);
}

void AutoCompiler::visitFuncDecl(FuncDecl<TypedLocationInfo>& decl) {
  uint8_t sigArgs[decl.args.size()];

  for (int i = 0; i < decl.args.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, decl.args.size());
  curFunc = cc.addFunc(sig);
  curFuncName = decl.name;

  for (int i = 0; i < decl.args.size(); ++i) {
    X86Gp arg = cc.newInt64();
    cc.setArg(i, arg);
    argRegs.push_back(arg);
  }

  typeErrorLabel = cc.newLabel();

  TypedLocationInfo endType;
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
    endType = (*it)->resolvedData;
  }

  if (vRegs.size()) {
    box(vRegs.top(), endType);
    cc.ret(vRegs.top());
  } else {
    X86Gp voidReg = cc.newInt64();
    cc.mov(voidReg, Value().getRaw());
    cc.ret(voidReg);
  }

  cc.bind(typeErrorLabel);
  cc.call((size_t) &typeError, FuncSignature0<void>());
  cc.endFunc();
  cc.finalize();

  funcs.addFunc(decl.name, &codeHolder, decl.args.size());
}

void AutoCompiler::visitExpressionStatement(ExpressionStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
}

void AutoCompiler::visitBlock(Block<TypedLocationInfo>& block) {
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = block.statements.begin();
      it != block.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
}

void AutoCompiler::visitAdditionExpression(AdditionExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result.r32(), left_loc.data.imm);
    cc.add(result.r32(), right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(rhsReg, expr.right->resolvedData);
    if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.add(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.add(result.r32(), right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    typecheckInt(rhsReg, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
      lhsReg = rhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.add(result.r32(), rhsReg.r32());
  }
  vRegs.push(result);
}

void AutoCompiler::visitSubtractionExpression(SubtractionExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result.r32(), left_loc.data.imm);
    cc.sub(result.r32(), right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(rhsReg, expr.right->resolvedData);
    if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.neg(result.r32());
    cc.add(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.sub(result.r32(), right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    typecheckInt(rhsReg, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
      cc.sub(result.r32(), rhsReg.r32());
    } else if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
      cc.neg(result.r32());
      cc.add(result.r32(), lhsReg.r32());
    } else {
      cc.mov(result, lhsReg);
      cc.add(result.r32(), lhsReg.r32());
    }
  }
  vRegs.push(result);
}
void AutoCompiler::visitMultiplicationExpression(MultiplicationExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  X86Gp result = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result.r32(), left_loc.data.imm);
    cc.imul(result.r32(), right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(rhsReg, expr.right->resolvedData);
    if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.imul(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.imul(result.r32(), right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    typecheckInt(rhsReg, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else if (right_loc.type != LOCATION_TYPE_ARG) {
      result = rhsReg;
      lhsReg = rhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.imul(result.r32(), rhsReg.r32());
  }
  vRegs.push(result);
}
void AutoCompiler::visitDivisionExpression(DivisionExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  if (expr.left->resolvedData.type == VAIVEN_STATIC_TYPE_UNKNOWN) {
  }

  X86Gp result = cc.newInt64();
  X86Gp dummy = cc.newInt64();
  cc.xor_(dummy, dummy);
  X86Gp divisor = cc.newInt64();
  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    cc.mov(result.r32(), left_loc.data.imm);
    cc.mov(divisor.r32(), right_loc.data.imm);
  } else if (leftImm) {
    expr.right->accept(*this);
    // will this work if its on the stack?
    divisor = vRegs.top(); vRegs.pop();
    typecheckInt(divisor, expr.right->resolvedData);
    cc.mov(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
    cc.mov(divisor.r32(), right_loc.data.imm);
  } else {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    expr.right->accept(*this);
    divisor = vRegs.top(); vRegs.pop();
    typecheckInt(lhsReg, expr.left->resolvedData);
    typecheckInt(divisor, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
  }
  cc.idiv(dummy.r32(), result.r32(), divisor.r32());
  vRegs.push(result);
}

void AutoCompiler::typecheckInt(asmjit::X86Gp vreg, TypedLocationInfo& typeInfo) {
  if (typeInfo.type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    X86Gp testReg = cc.newInt64();
    cc.mov(testReg, vreg);
    cc.shr(testReg, INT_TAG_SHIFT);
    cc.cmp(testReg, INT_TAG_SHIFTED);
    cc.jne(typeErrorLabel);
  } else if (typeInfo.type != VAIVEN_STATIC_TYPE_INT) {
    typeError();
  }
}

void AutoCompiler::box(asmjit::X86Gp vReg, TypedLocationInfo& typeInfo) {
  if (typeInfo.isBoxed) {
    return;
  }

  if (typeInfo.type == VAIVEN_STATIC_TYPE_INT) {
    // can't use 64 bit immediates except with MOV
    X86Gp fullValue = cc.newInt64();
    cc.mov(fullValue, INT_TAG);
    cc.or_(vReg, fullValue);
  } else if (typeInfo.type == VAIVEN_STATIC_TYPE_BOOL) {
    X86Gp fullValue = cc.newInt64();
    cc.mov(fullValue, BOOL_TAG);
    cc.or_(vReg, fullValue);
  } else if (typeInfo.type == VAIVEN_STATIC_TYPE_DOUBLE) {
    X86Gp fullValue = cc.newInt64();
    cc.mov(fullValue, (uint64_t) 0xFFFFFFFFFFFFFFFF);
    // no bitwise negation, so do 1111 XOR ????
    cc.xor_(vReg, fullValue);
  }
  // POINTER nothing to do
  // UNKNOWN should never happen
  // VOID should never happen
}

void AutoCompiler::visitIntegerExpression(IntegerExpression<TypedLocationInfo>& expr) {
  // should only happen when in a stmt by itself
  X86Gp var = cc.newInt64();
  cc.mov(var, Value(expr.value).getRaw());
  vRegs.push(var);
}

void AutoCompiler::visitVariableExpression(VariableExpression<TypedLocationInfo>& expr) {
  if (scope.contains(expr.id)) {
    vRegs.push(scope.get(expr.id));
  } else {
    vRegs.push(argRegs[expr.resolvedData.location.data.argIndex]);
  }
}
