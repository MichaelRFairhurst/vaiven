#ifndef VAIVEN_HEADER_FUNCTIONS
#define VAIVEN_HEADER_FUNCTIONS

#include <map>
#include <memory>
#include <string>

#include "asmjit/src/asmjit/asmjit.h"

using std::map;
using std::string;
using std::unique_ptr;

namespace vaiven {

typedef int64_t (*OverkillFunc)(int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t r9, int64_t stack1, int64_t stack2);

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
