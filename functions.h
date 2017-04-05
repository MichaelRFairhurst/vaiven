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
  Function(asmjit::JitRuntime* runtime,
      int argc,
      unique_ptr<FunctionUsage> usage,
      ast::FuncDecl<>* ast)
    : runtime(runtime), argc(argc), usage(std::move(usage)), ast(ast), isPure(false), isNative(false) {};

  ~Function() {
    if (!isNative) {
      runtime->release(fptr);
    }
  }

  bool isNative;
  int argc;
  int worstSize;
  bool isPure; // for native functions
  OverkillFunc fptr;
  OverkillFunc slowfptr;
  unique_ptr<FunctionUsage> usage;
  unique_ptr<ast::FuncDecl<> > ast;

  private:
  asmjit::JitRuntime* runtime;
};

class DuplicateFunctionError {
  public:
  DuplicateFunctionError(string name) : name(name) {};
  string name;
};

class Functions {
  public:
  asmjit::JitRuntime runtime;

  void addNative(string name, int argc, void* fptr, bool isPure) {
    funcs[name] = unique_ptr<Function>(new Function(NULL, argc, unique_ptr<FunctionUsage>(), NULL));
    funcs[name]->isNative = true;
    funcs[name]->fptr = (OverkillFunc) fptr;
    funcs[name]->isPure = isPure;
  }

  void prepareFunc(string name, int argc, unique_ptr<FunctionUsage> usage,
      ast::FuncDecl<>* ast) {
    if (funcs.find(name) != funcs.end()) {
      throw DuplicateFunctionError(name);
    }
    funcs[name] = unique_ptr<Function>(new Function(&runtime, argc, std::move(usage), ast));
  }

  void finalizeFunc(string name, asmjit::CodeHolder* holder) {
    int worstSize = holder->getCodeSize();
    runtime.add(&funcs[name]->fptr, holder);
    funcs[name]->worstSize = worstSize;
  }

  // private:
  map<string, unique_ptr<Function> > funcs;
};

}

#endif
