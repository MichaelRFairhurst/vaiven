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

  std::cout << "optimizing " << funcDecl.name << std::endl;
  for (int i = 0; i < funcDecl.args.size(); ++i) {
    std::cout << "arg " << i << funcs.funcs[funcDecl.name]->usage->argShapes[i].isPure() << std::endl;
  }

  PrintErrorHandler2 eh;
  FileLogger logger(stdout);
  CodeHolder codeHolder;
  codeHolder.init(funcs.runtime.getCodeInfo());
  codeHolder.setErrorHandler(&eh);
  codeHolder.setLogger(&logger);
  X86Assembler assembler(&codeHolder);
  X86Compiler cc(&codeHolder);

  vaiven::visitor::UsageApplier applier(*funcs.funcs[funcDecl.name]->usage);
  funcDecl.accept(applier);
  
  visitor::ReCompiler compiler(cc, codeHolder, funcs, *funcs.funcs[funcDecl.name]->usage);
  compiler.compile(funcDecl);

  std::cout << "done optimizing" << std::endl;
  return funcs.funcs[funcDecl.name]->fptr;
}

