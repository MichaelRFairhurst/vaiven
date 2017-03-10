#ifndef VAIVEN_VISITOR_HEADER_ERROR_COMPILER
#define VAIVEN_VISITOR_HEADER_ERROR_COMPILER

#include "type_info.h"
#include "value.h"
#include "asmjit/src/asmjit/asmjit.h"

namespace vaiven {

using asmjit::X86Compiler;

class ErrorCompiler {

  public:
  ErrorCompiler(X86Compiler& cc) : cc(cc),
    hasIntTypeError(false), hasBoolTypeError(false), hasNoFuncError(false),
    hasNoVarError(false), hasDupVarError(false), intTypeErrorLabel(cc.newLabel()),
    boolTypeErrorLabel(cc.newLabel()), noFuncErrorLabel(cc.newLabel()),
    noVarErrorLabel(cc.newLabel()), dupVarErrorLabel(cc.newLabel()) {}

  void generateTypeErrorProlog();
  void typecheckInt(asmjit::X86Gp vreg, TypedLocationInfo& info);
  void typecheckBool(asmjit::X86Gp vreg, TypedLocationInfo& info);

  X86Compiler& cc;


  void intTypeError();
  void boolTypeError();
  void noFuncError();
  void noVarError();
  void dupVarError();

  asmjit::Label intTypeErrorLabel;
  bool hasIntTypeError;

  asmjit::Label boolTypeErrorLabel;
  bool hasBoolTypeError;

  asmjit::Label noFuncErrorLabel;
  bool hasNoFuncError;

  asmjit::Label noVarErrorLabel;
  bool hasNoVarError;

  asmjit::Label dupVarErrorLabel;
  bool hasDupVarError;

};

}

#endif
