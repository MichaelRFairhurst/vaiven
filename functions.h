#ifndef VAIVEN_HEADER_FUNCTIONS
#define VAIVEN_HEADER_FUNCTIONS

#include <map>
#include <memory>
#include <string>

#include "asmjit/src/asmjit/asmjit.h"

#include "value.h"

using std::map;
using std::string;
using std::unique_ptr;

namespace vaiven {

typedef Value (*OverkillFunc)(Value rdi, Value rsi, Value rdx, Value rcx, Value r8, Value r9, Value stack1, Value stack2);

class Function {
  public: 
  Function(const Function& that) = delete;
  Function(asmjit::JitRuntime& runtime, OverkillFunc fptr, int argc) : runtime(runtime), fptr(fptr), argc(argc) {};

  ~Function() {
    runtime.release(fptr);
  }

  int argc;
  OverkillFunc fptr;

  private:
  asmjit::JitRuntime& runtime;
};

class Functions {
  public:
  asmjit::JitRuntime runtime;

  void addFunc(string name, asmjit::CodeHolder* holder, int argc) {
    OverkillFunc fptr;
    runtime.add(&fptr, holder);
    funcs[name] = unique_ptr<Function>(new Function(runtime, fptr, argc));
  }

  // private:
  map<string, unique_ptr<Function> > funcs;
};

}

#endif
