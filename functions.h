#ifndef VAIVEN_HEADER_FUNCTIONS
#define VAIVEN_HEADER_FUNCTIONS

#include <map>
#include <memory>
#include <string>

#include "asmjit/src/asmjit/asmjit.h"

#include "value.h"
#include "function_usage.h"
#include "type_info.h"
#include "ast/funcdecl.h"

using std::map;
using std::string;
using std::unique_ptr;

namespace vaiven {

typedef Value (*OverkillFunc)(Value rdi, Value rsi, Value rdx, Value rcx, Value r8, Value r9, Value stack1, Value stack2);

class Function {
  public: 
  Function(const Function& that) = delete;
  Function(asmjit::JitRuntime& runtime, OverkillFunc fptr, int argc, unique_ptr<FunctionUsage> usage, ast::FuncDecl<TypedLocationInfo>* ast)
    : runtime(runtime), fptr(fptr), argc(argc), usage(std::move(usage)), ast(ast) {};

  ~Function() {
    runtime.release(fptr);
  }

  int argc;
  OverkillFunc fptr;
  OverkillFunc slowfptr;
  unique_ptr<FunctionUsage> usage;
  unique_ptr<ast::FuncDecl<TypedLocationInfo> > ast;

  private:
  asmjit::JitRuntime& runtime;
};

class Functions {
  public:
  asmjit::JitRuntime runtime;

  void addFunc(string name, asmjit::CodeHolder* holder, int argc, unique_ptr<FunctionUsage> usage, ast::FuncDecl<TypedLocationInfo>* ast) {
    OverkillFunc fptr;
    runtime.add(&fptr, holder);
    funcs[name] = unique_ptr<Function>(new Function(runtime, fptr, argc, std::move(usage), ast));
  }

  // private:
  map<string, unique_ptr<Function> > funcs;
};

}

#endif
