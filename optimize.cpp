#include "optimize.h"

#include "stdio.h"
#include <iostream>

#include "visitor/recompiler.h"
#include "visitor/usage_applier.h"
#include "asmjit/src/asmjit/asmjit.h"

using namespace vaiven;
using namespace asmjit;

// Error handler that just prints the error and lets AsmJit ignore it.
class PrintErrorHandler2 : public asmjit::ErrorHandler {
public:
  // Return `true` to set last error to `err`, return `false` to do nothing.
  bool handleError(asmjit::Error err, const char* message, asmjit::CodeEmitter* origin) override {
    fprintf(stderr, "ERROR: %s\n", message);
    return false;
  }
};

OverkillFunc vaiven::optimize(vaiven::Functions& funcs, ast::FuncDecl<vaiven::TypedLocationInfo>& funcDecl) {
  if (funcs.funcs.find(funcDecl.name) == funcs.funcs.end()) {
    throw "func not known";
  }

#ifdef OPTIMIZATION_DIAGNOSTICS
  std::cout << "optimizing " << funcDecl.name << std::endl;
#endif

  CodeHolder codeHolder;
  codeHolder.init(funcs.runtime.getCodeInfo());
#ifdef DISASSEMBLY_DIAGNOSTICS
  PrintErrorHandler2 eh;
  FileLogger logger(stdout);
  codeHolder.setErrorHandler(&eh);
  codeHolder.setLogger(&logger);
#endif
  X86Assembler assembler(&codeHolder);
  X86Compiler cc(&codeHolder);

  vaiven::visitor::UsageApplier applier(*funcs.funcs[funcDecl.name]->usage);
  funcDecl.accept(applier);
  
  visitor::ReCompiler compiler(cc, codeHolder, funcs, *funcs.funcs[funcDecl.name]->usage);
  compiler.compile(funcDecl);

#ifdef OPTIMIZATION_DIAGNOSTICS
  std::cout << "done optimizing" << std::endl;
#endif
  return funcs.funcs[funcDecl.name]->fptr;
}

