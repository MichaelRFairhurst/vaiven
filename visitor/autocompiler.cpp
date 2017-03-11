#include "autocompiler.h"

#include "../ast/all.h"
#include "../value.h"
#include "../runtime_error.h"
#include "../optimize.h"
#include "jumping_compiler.h"

#include <iostream>
#include <stdint.h>

using namespace asmjit;
using namespace vaiven::visitor;

void AutoCompiler::compile(Node<TypedLocationInfo>& root) {
  root.accept(*this);
}

void AutoCompiler::visitIfStatement(IfStatement<TypedLocationInfo>& stmt) {
  Label lfalse = cc.newLabel();
  Label lafter = cc.newLabel();
  JumpingCompiler jc(cc, *this, lfalse, true /* jump on false */);
  stmt.condition->accept(jc);

  if (!jc.didJmp) {
    error.typecheckBool(vRegs.top(), stmt.condition->resolvedData);
    cc.cmp(vRegs.top().r32(), 0);
    vRegs.pop();
    cc.je(lfalse);
  }

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

void AutoCompiler::visitForCondition(ForCondition<TypedLocationInfo>& stmt) {
  Label lcheck = cc.newLabel();
  Label lafter = cc.newLabel();
  cc.bind(lcheck);

  JumpingCompiler jc(cc, *this, lafter, true /* jump on false */);
  stmt.condition->accept(jc);

  if (!jc.didJmp) {
    error.typecheckBool(vRegs.top(), stmt.condition->resolvedData);
    cc.cmp(vRegs.top().r32(), 0);
    vRegs.pop();
    cc.je(lafter);
  }

  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = stmt.statements.begin();
      it != stmt.statements.end();
      ++it) {
    (*it)->accept(*this);
  }
  cc.jmp(lcheck);
  cc.bind(lafter);
}

void AutoCompiler::visitReturnStatement(ReturnStatement<TypedLocationInfo>& stmt) {
  stmt.expr->accept(*this);
  box(vRegs.top(), stmt.expr->resolvedData);
  cc.ret(vRegs.top());
}

void AutoCompiler::visitVarDecl(VarDecl<TypedLocationInfo>& varDecl) {
  varDecl.expr->accept(*this);

  if (varDecl.resolvedData.location.type == LOCATION_TYPE_ARG) {
    error.dupVarError();
    return;
  }

  X86Gp varReg = cc.newInt64();
  scope.put(varDecl.varname, varReg);
  cc.mov(varReg, vRegs.top());
  box(varReg, varDecl.expr->resolvedData);
  vRegs.pop();
}

void AutoCompiler::visitFuncCallExpression(FuncCallExpression<TypedLocationInfo>& expr) {
  if (expr.name != curFuncName && funcs.funcs.find(expr.name) == funcs.funcs.end()) {
    error.noFuncError();
    vRegs.push(cc.newUInt64());
    return;
  }

  int argc = funcs.funcs[expr.name]->argc;
  int paramc = expr.parameters.size();

  uint8_t sigArgs[argc];
  vector<X86Gp> paramRegs;

  for (int i = 0; i < paramc; ++i) {
    expr.parameters[i]->accept(*this);
    paramRegs.push_back(vRegs.top());
    if (i < argc) {
      // box those that are actually passed in :)
      box(vRegs.top(), expr.parameters[i]->resolvedData);
    }
    vRegs.pop();
  }

  // unspecified void values
  for (int i = paramc; i < argc; ++i) {
    X86Gp void_ = cc.newUInt64();
    cc.mov(void_, VOID);
    paramRegs.push_back(void_);
  }

  for (int i = 0; i < argc; ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, argc);

  // careful that this always handles self-optimization
  X86Gp lookup = cc.newUInt64();
  cc.mov(lookup, (uint64_t) &funcs.funcs[expr.name]->fptr);
  CCFuncCall* call = cc.call(x86::ptr(lookup), sig);

  for (int i = 0; i < argc; ++i) {
    call->setArg(i, paramRegs[i]);
  }

  X86Gp retReg = cc.newInt64();
  call->setRet(0, retReg);
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

  // allocate a variably sized FunctionUsage with room for shapes
  void* usageMem = malloc(sizeof(FunctionUsage) + sizeof(ArgumentShape) * decl.args.size());
  FunctionUsage* usage = (FunctionUsage*) usageMem;
  unique_ptr<FunctionUsage> savedUsage(new (usage) FunctionUsage());

  // prepare it so it knows how to recurse
  funcs.prepareFunc(decl.name, decl.args.size(), std::move(savedUsage), &decl);

  X86Gp checkReg = cc.newUInt64();
  X86Gp orReg = cc.newUInt64();
  for (int i = 0; i < decl.args.size(); ++i) {
    X86Gp arg = cc.newInt64();
    cc.setArg(i, arg);
    argRegs.push_back(arg);
  }

  generateTypeShapePrelog(decl, usage);

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

  generateOptimizeProlog(decl, sig);
  error.generateTypeErrorProlog();

  cc.endFunc();
  cc.finalize();

  funcs.finalizeFunc(decl.name, &codeHolder);
}

void AutoCompiler::generateTypeShapePrelog(FuncDecl<TypedLocationInfo>& decl, FunctionUsage* usage) {
  if (!decl.args.size()) {
    return;
  }

  optimizeLabel = cc.newLabel();
  X86Gp count = cc.newInt32();
  cc.mov(count, asmjit::x86::dword_ptr((uint64_t) &usage->count));
  cc.add(count, 1);
  cc.mov(asmjit::x86::dword_ptr((uint64_t) &usage->count), count);
  cc.cmp(count, HOT_COUNT);
  cc.je(optimizeLabel);

  X86Gp checkReg = cc.newUInt64();
  X86Gp orReg = cc.newUInt64();
  for (int i = 0; i < decl.args.size(); ++i) {
    usage->argShapes[i].raw = 0; // initialize
    X86Gp arg = argRegs[i];

    Label afterCheck = cc.newLabel();

    cc.mov(checkReg, MAX_PTR);
    cc.mov(orReg, OBJECT_SHAPE);
    cc.cmp(arg, checkReg);
    cc.jl(afterCheck);

    cc.mov(checkReg, MIN_DBL);
    cc.mov(orReg, DOUBLE_SHAPE);
    cc.cmp(arg, checkReg);
    cc.jg(afterCheck);

    cc.mov(orReg, arg);
    cc.shr(orReg, 48);
    // creates a tag for ints, bools, and void

    cc.bind(afterCheck);
    cc.mov(checkReg.r16(), x86::word_ptr((uint64_t) &usage->argShapes[i]));
    cc.or_(checkReg.r16(), orReg.r16());
    cc.mov(x86::word_ptr((uint64_t) &usage->argShapes[i]), checkReg.r16());
  }
}

void AutoCompiler::generateOptimizeProlog(FuncDecl<TypedLocationInfo>& decl, FuncSignature& sig) {
  if (!decl.args.size()) {
    return;
  }

  cc.bind(optimizeLabel);
  X86Gp funcsReg = cc.newUInt64();
  X86Gp declReg = cc.newUInt64();
  X86Gp optimizedAddr = cc.newUInt64();
  cc.mov(funcsReg, (uint64_t) &funcs);
  cc.mov(declReg, (uint64_t) &decl);
  CCFuncCall* recompileCall = cc.call((size_t) &vaiven::optimize, FuncSignature2<uint64_t, uint64_t, uint64_t>());
  recompileCall->setArg(0, funcsReg);
  recompileCall->setArg(1, declReg);
  recompileCall->setRet(0, optimizedAddr);

  // hack bugfix workaround
  vector<X86Gp> argWorkarounds;
  for (int i = 0; i < decl.args.size(); ++i) {
    X86Gp argRegWorkaround = cc.newUInt64();
    cc.mov(argRegWorkaround, argRegs[i]);
    argWorkarounds.push_back(argRegWorkaround);
  }
  CCFuncCall* optimizedCall = cc.call(optimizedAddr, sig);
  for (int i = 0; i < decl.args.size(); ++i) {
    optimizedCall->setArg(i, argWorkarounds[i]);
    //optimizedCall->setArg(i, argRegs[i]);
  }
  X86Gp optimizedRet = cc.newUInt64();
  optimizedCall->setRet(0, optimizedRet);
  
  cc.ret(optimizedRet);
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

void AutoCompiler::visitAssignmentExpression(AssignmentExpression<TypedLocationInfo>& expr) {
  expr.expr->accept(*this);
  X86Gp target;
  if (scope.contains(expr.varname)) {
    target = scope.get(expr.varname);
  } else {
    int argIndex = expr.resolvedData.location.data.argIndex;
    if (argIndex == -1) {
      error.noVarError();
      return;
    } else {
      target = argRegs[expr.resolvedData.location.data.argIndex];
    }
  }
  box(vRegs.top(), expr.expr->resolvedData);
  cc.mov(target, vRegs.top());
  vRegs.pop();
  vRegs.push(target);
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
    error.typecheckInt(rhsReg, expr.right->resolvedData);
    if (right_loc.type != LOCATION_TYPE_ARG && right_loc.type != LOCATION_TYPE_LOCAL) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.add(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
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
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    error.typecheckInt(rhsReg, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
      result = lhsReg;
    } else if (right_loc.type != LOCATION_TYPE_ARG && right_loc.type != LOCATION_TYPE_LOCAL) {
      result = rhsReg;
      rhsReg = lhsReg;
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
    error.typecheckInt(rhsReg, expr.right->resolvedData);
    if (right_loc.type != LOCATION_TYPE_ARG && right_loc.type != LOCATION_TYPE_LOCAL) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.neg(result.r32());
    cc.add(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
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
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    error.typecheckInt(rhsReg, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
      result = lhsReg;
      cc.sub(result.r32(), rhsReg.r32());
    } else if (right_loc.type != LOCATION_TYPE_ARG && right_loc.type != LOCATION_TYPE_LOCAL) {
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
    error.typecheckInt(rhsReg, expr.right->resolvedData);
    if (right_loc.type != LOCATION_TYPE_ARG && right_loc.type != LOCATION_TYPE_LOCAL) {
      result = rhsReg;
    } else {
      cc.mov(result, rhsReg);
    }
    cc.imul(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
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
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    error.typecheckInt(rhsReg, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
      result = lhsReg;
    } else if (right_loc.type != LOCATION_TYPE_ARG && right_loc.type != LOCATION_TYPE_LOCAL) {
      result = rhsReg;
      rhsReg = lhsReg;
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

  X86Gp result = cc.newInt64();
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
    error.typecheckInt(divisor, expr.right->resolvedData);
    cc.mov(result.r32(), left_loc.data.imm);
  } else if (rightImm) {
    expr.left->accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
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
    error.typecheckInt(lhsReg, expr.left->resolvedData);
    error.typecheckInt(divisor, expr.right->resolvedData);
    if (left_loc.type != LOCATION_TYPE_ARG && left_loc.type != LOCATION_TYPE_LOCAL) {
      result = lhsReg;
    } else {
      cc.mov(result, lhsReg);
    }
  }
  X86Gp dummy = cc.newInt64();
  cc.xor_(dummy, dummy);
  cc.idiv(dummy.r32(), result.r32(), divisor.r32());
  vRegs.push(result);
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
    int argIndex = expr.resolvedData.location.data.argIndex;
    if (argIndex == -1) {
      error.noVarError();
      vRegs.push(cc.newUInt64());
    } else {
      vRegs.push(argRegs[expr.resolvedData.location.data.argIndex]);
    }
  }
}

void AutoCompiler::visitBoolLiteral(BoolLiteral<TypedLocationInfo>& expr) {
  // should only happen when in a stmt by itself
  X86Gp var = cc.newInt64();
  cc.mov(var, Value(expr.value).getRaw());
  vRegs.push(var);
}

void AutoCompiler::doCmpNotExpression(NotExpression<TypedLocationInfo>& expr) {
  Location& inner_loc = expr.expr->resolvedData.location;
  bool innerImm = inner_loc.type == LOCATION_TYPE_IMM;

  if (innerImm) {
    X86Gp tmp = cc.newInt64();
    cc.mov(tmp.r32(), inner_loc.data.imm);
    cc.test(tmp, tmp);
  } else {
    expr.expr->accept(*this);
    X86Gp valReg = vRegs.top(); vRegs.pop();
    error.typecheckBool(valReg, expr.expr->resolvedData);
    cc.test(valReg.r32(), valReg.r32());
  }
}

void AutoCompiler::visitNotExpression(NotExpression<TypedLocationInfo>& expr) {
  X86Gp result = cc.newInt64();
  cc.xor_(result, result);
  doCmpNotExpression(expr);
  cc.setz(result);
  vRegs.push(result);
}

void AutoCompiler::visitInequalityExpression(InequalityExpression<TypedLocationInfo>& expr) {
  X86Gp result = cc.newInt64();
  cc.xor_(result, result);
  doCmpEqualityExpression(*expr.left, *expr.right);
  cc.setne(result);
  vRegs.push(result);
}

void AutoCompiler::doCmpEqualityExpression(Expression<TypedLocationInfo>& left, Expression<TypedLocationInfo>& right) {
  Location& left_loc = left.resolvedData.location;
  Location& right_loc = right.resolvedData.location;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    X86Gp tmp = cc.newInt64();
    cc.mov(tmp.r32(), left_loc.data.imm);
    cc.cmp(tmp.r32(), right_loc.data.imm);
  } else if (leftImm) {
    right.accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    if (left.resolvedData.type == VAIVEN_STATIC_TYPE_INT
        || left.resolvedData.type == VAIVEN_STATIC_TYPE_BOOL) {
      cc.cmp(rhsReg.r32(), left_loc.data.imm);
    }
    // TODO objects, doubles
  } else if (rightImm) {
    left.accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    if (right.resolvedData.type == VAIVEN_STATIC_TYPE_INT
        || right.resolvedData.type == VAIVEN_STATIC_TYPE_BOOL) {
      cc.cmp(lhsReg.r32(), right_loc.data.imm);
    }
    // TODO objects, doubles
  } else {
    left.accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    right.accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    cc.cmp(lhsReg, rhsReg);
  }
}

void AutoCompiler::visitEqualityExpression(EqualityExpression<TypedLocationInfo>& expr) {
  X86Gp result = cc.newInt64();
  cc.xor_(result, result);
  doCmpEqualityExpression(*expr.left, *expr.right);
  cc.sete(result);
  vRegs.push(result);
}

void AutoCompiler::visitGtExpression(GtExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type != LOCATION_TYPE_IMM && right_loc.type == LOCATION_TYPE_IMM;
  X86Gp result = cc.newUInt64();
  cc.xor_(result, result);
  doCmpIntExpression(*expr.left, *expr.right);
  if (backwardsCmp) {
    cc.setl(result);
  } else {
    cc.setg(result);
  }
  vRegs.push(result);
}

void AutoCompiler::doCmpIntExpression(Expression<TypedLocationInfo>& left, Expression<TypedLocationInfo>& right) {
  Location& left_loc = left.resolvedData.location;
  Location& right_loc = right.resolvedData.location;
  bool leftImm = left_loc.type == LOCATION_TYPE_IMM;
  bool rightImm = right_loc.type == LOCATION_TYPE_IMM;

  if (leftImm && rightImm) {
    // mov rax, lhsImm or exact reg
    // add rax, rhsImm or exact reg
    X86Gp tmp = cc.newInt64();
    cc.mov(tmp.r32(), left_loc.data.imm);
    cc.cmp(tmp.r32(), right_loc.data.imm);
  } else if (leftImm) {
    right.accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    error.typecheckInt(rhsReg, right.resolvedData);
    cc.cmp(rhsReg.r32(), left_loc.data.imm);
  } else if (rightImm) {
    left.accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    error.typecheckInt(lhsReg, left.resolvedData);
    cc.cmp(lhsReg.r32(), right_loc.data.imm);
  } else {
    left.accept(*this);
    X86Gp lhsReg = vRegs.top(); vRegs.pop();
    right.accept(*this);
    X86Gp rhsReg = vRegs.top(); vRegs.pop();
    error.typecheckInt(rhsReg, right.resolvedData);
    error.typecheckInt(lhsReg, left.resolvedData);
    cc.cmp(lhsReg, rhsReg);
  }
}

void AutoCompiler::visitGteExpression(GteExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type != LOCATION_TYPE_IMM && right_loc.type == LOCATION_TYPE_IMM;
  X86Gp result = cc.newUInt64();
  cc.xor_(result, result);
  doCmpIntExpression(*expr.left, *expr.right);
  if (backwardsCmp) {
    cc.setle(result);
  } else {
    cc.setge(result);
  }
  vRegs.push(result);
}

void AutoCompiler::visitLtExpression(LtExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type != LOCATION_TYPE_IMM && right_loc.type == LOCATION_TYPE_IMM;
  X86Gp result = cc.newUInt64();
  cc.xor_(result, result);
  doCmpIntExpression(*expr.left, *expr.right);
  if (backwardsCmp) {
    cc.setg(result);
  } else {
    cc.setl(result);
  }
  vRegs.push(result);
}

void AutoCompiler::visitLteExpression(LteExpression<TypedLocationInfo>& expr) {
  Location& left_loc = expr.left->resolvedData.location;
  Location& right_loc = expr.right->resolvedData.location;
  bool backwardsCmp = left_loc.type != LOCATION_TYPE_IMM && right_loc.type == LOCATION_TYPE_IMM;
  X86Gp result = cc.newUInt64();
  cc.xor_(result, result);
  doCmpIntExpression(*expr.left, *expr.right);
  if (backwardsCmp) {
    cc.setge(result);
  } else {
    cc.setle(result);
  }
  vRegs.push(result);
}
