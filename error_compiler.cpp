#include "error_compiler.h"
#include "runtime_error.h"

using namespace asmjit;

void vaiven::ErrorCompiler::intTypeError() {
  cc.jmp(intTypeErrorLabel);
  hasIntTypeError = true;
}

void vaiven::ErrorCompiler::boolTypeError() {
  cc.jmp(boolTypeErrorLabel);
  hasBoolTypeError = true;
}

void vaiven::ErrorCompiler::stringTypeError() {
  cc.jmp(stringTypeErrorLabel);
  hasStringTypeError = true;
}

void vaiven::ErrorCompiler::noFuncError() {
  cc.jmp(noFuncErrorLabel);
  hasNoFuncError = true;
}

void vaiven::ErrorCompiler::noVarError() {
  cc.jmp(noVarErrorLabel);
  hasNoVarError = true;
}

void vaiven::ErrorCompiler::dupVarError() {
  cc.jmp(dupVarErrorLabel);
  hasDupVarError = true;
}

void vaiven::ErrorCompiler::generateTypeErrorProlog() {
  if (hasIntTypeError) {
    cc.bind(intTypeErrorLabel);
    cc.call((size_t) &expectedInt, FuncSignature0<void>());
  }

  if (hasBoolTypeError) {
    cc.bind(boolTypeErrorLabel);
    cc.call((size_t) &expectedBool, FuncSignature0<void>());
  }

  if (hasStringTypeError) {
    cc.bind(stringTypeErrorLabel);
    cc.call((size_t) &expectedStr, FuncSignature0<void>());
  }

  if (hasNoFuncError) {
    cc.bind(noFuncErrorLabel);
    cc.call((size_t) &noSuchFunction, FuncSignature0<void>());
  }

  if (hasNoVarError) {
    cc.bind(noVarErrorLabel);
    cc.call((size_t) &noSuchVar, FuncSignature0<void>());
  }

  if (hasDupVarError) {
    cc.bind(dupVarErrorLabel);
    cc.call((size_t) &duplicateVar, FuncSignature0<void>());
  }
}

void vaiven::ErrorCompiler::typecheckInt(asmjit::X86Gp vreg, TypedLocationInfo& typeInfo) {
  if (typeInfo.type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    X86Gp testReg = cc.newInt64();
    cc.mov(testReg, vreg);
    cc.shr(testReg, VALUE_TAG_SHIFT);
    cc.cmp(testReg, INT_TAG_SHIFTED);
    cc.jne(intTypeErrorLabel);

    hasIntTypeError = true;
  } else if (typeInfo.type != VAIVEN_STATIC_TYPE_INT) {
    cc.jmp(intTypeErrorLabel);
    hasIntTypeError = true;
  }
}

void vaiven::ErrorCompiler::typecheckBool(asmjit::X86Gp vreg, TypedLocationInfo& typeInfo) {
  if (typeInfo.type == VAIVEN_STATIC_TYPE_UNKNOWN) {
    X86Gp testReg = cc.newInt64();
    cc.mov(testReg, vreg);
    cc.shr(testReg, VALUE_TAG_SHIFT);
    cc.cmp(testReg, BOOL_TAG_SHIFTED);
    cc.jne(boolTypeErrorLabel);

    hasBoolTypeError = true;
  } else if (typeInfo.type != VAIVEN_STATIC_TYPE_BOOL) {
    cc.jmp(intTypeErrorLabel);
    hasBoolTypeError = true;
  }
}
