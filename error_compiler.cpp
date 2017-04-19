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

void vaiven::ErrorCompiler::generateTypeErrorEpilog() {
  if (hasIntTypeError) {
    cc.bind(intTypeErrorLabel);
    cc.call((size_t) &expectedInt, FuncSignature0<void>());
  }

  if (hasDoubleConvertableTypeError) {
    cc.bind(doubleConvertableTypeErrorLabel);
    cc.call((size_t) &expectedIntOrDouble, FuncSignature0<void>());
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
