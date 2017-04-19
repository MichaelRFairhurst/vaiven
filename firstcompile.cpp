#include "firstcompile.h"

#include "stdio.h"
#include <iostream>

#include "optimize.h"
#include "asmjit/src/asmjit/asmjit.h"
#include "ast/all.h"
#include "value.h"
#include "runtime_error.h"
#include "visitor/ssa_builder.h"
#include "ssa/emitter.h"
#include "ssa/reg_alloc.h"
#include "ssa/print_visitor.h"
#include "ssa/constant_propagation.h"
#include "ssa/instruction_combiner.h"
#include "ssa/unused_code.h"
#include "ssa/type_analysis.h"
#include "ssa/constant_inliner.h"
#include "ssa/jmp_threader.h"
#include "ssa/inliner.h"
#include "ssa/dominator_builder.h"
#include "ssa/loop_invariant.h"
#include "ssa/common_subexpression.h"

#include <iostream>
#include <stdint.h>

using namespace asmjit;
using namespace vaiven::visitor;

using namespace vaiven;
using namespace asmjit;

// Error handler that just prints the error and lets AsmJit ignore it.
class PrintErrorHandler3 : public asmjit::ErrorHandler {
public:
  // Return `true` to set last error to `err`, return `false` to do nothing.
  bool handleError(asmjit::Error err, const char* message, asmjit::CodeEmitter* origin) override {
    fprintf(stderr, "Error: %s\n", message);
    return false;
  }
};

void vaiven::firstCompile(vaiven::Functions& funcs, ast::FuncDecl<>& funcDecl) {
#ifdef FIRST_COMPILE_DIAGNOSTICS
  std::cout << "compiling " << funcDecl.name << std::endl;
#endif

  CodeHolder codeHolder;
  codeHolder.init(funcs.runtime.getCodeInfo());

#ifdef DISASSEMBLY_DIAGNOSTICS
  PrintErrorHandler3 eh;
  FileLogger logger(stdout);
  codeHolder.setErrorHandler(&eh);
  codeHolder.setLogger(&logger);
#endif

  X86Assembler assembler(&codeHolder);
  X86Compiler cc(&codeHolder);


  vector<X86Gp> argRegs;
  uint8_t sigArgs[funcDecl.args.size()];

  for (int i = 0; i < funcDecl.args.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, funcDecl.args.size());
  CCFunc* curFunc = cc.addFunc(sig);
  curFunc->getFrameInfo().addAttributes(asmjit::FuncFrameInfo::kAttrPreserveFP);

  // allocate a variably sized FunctionUsage with room for shapes
  void* usageMem = malloc(sizeof(FunctionUsage) + sizeof(ArgumentShape) * funcDecl.args.size());
  FunctionUsage* usage = (FunctionUsage*) usageMem;
  unique_ptr<FunctionUsage> savedUsage(new (usage) FunctionUsage());

  // prepare it so it knows how to recurse
  funcs.prepareFunc(funcDecl.name, funcDecl.args.size(), std::move(savedUsage), &funcDecl);

  for (int i = 0; i < funcDecl.args.size(); ++i) {
    X86Gp arg = cc.newInt64();
    cc.setArg(i, arg);
    argRegs.push_back(arg);
  }

  Label optimizeLabel;
  generateTypeShapeProlog(funcDecl, usage, cc, &optimizeLabel, argRegs);

  ErrorCompiler error(cc);
  firstOptimizations(funcDecl, funcs, *usage, cc, argRegs, error, curFunc->getLabel());

  generateOptimizeEpilog(funcDecl, sig, cc, optimizeLabel, argRegs, funcs);
  error.generateTypeErrorEpilog();

  cc.endFunc();
  cc.finalize();

  funcs.finalizeFunc(funcDecl.name, &codeHolder);

#ifdef FIRST_COMPILE_DIAGNOSTICS
  std::cout << "done first compiling" << std::endl;
#endif
}

void vaiven::firstOptimizations(ast::FuncDecl<>& decl, Functions& funcs, FunctionUsage& usageInfo, asmjit::X86Compiler& cc, vector<X86Gp>& argRegs, ErrorCompiler& error, Label funcLabel) {
  SsaBuilder builder(usageInfo, funcs);
  decl.accept(builder);
#ifdef SSA_DIAGNOSTICS
  ssa::PrintVisitor printer;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  ssa::TypeAnalysis typeAnalysis;
  builder.head.accept(typeAnalysis);
#ifdef SSA_DIAGNOSTICS
  std::cout << "type analysis" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  while(true) {
    ssa::ConstantPropagator constantProp;
    builder.head.accept(constantProp);
#ifdef SSA_DIAGNOSTICS
    std::cout << "constant prop" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    ssa::UnusedCodeEliminator unusedCodeElim;
    builder.head.accept(unusedCodeElim);
#ifdef SSA_DIAGNOSTICS
    std::cout << "unused val elim" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    if (!unusedCodeElim.performedWork && !constantProp.performedWork) {
      break;
    }

  }

  ssa::ConstantInliner constInliner;
  builder.head.accept(constInliner);
#ifdef SSA_DIAGNOSTICS
  std::cout << "const inliner" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  ssa::JmpThreader jmpThreader;
  builder.head.accept(jmpThreader);
#ifdef SSA_DIAGNOSTICS
  std::cout << "jmp threading" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  while(true) {
    ssa::UnusedCodeEliminator unusedCodeElim;
    builder.head.accept(unusedCodeElim);
#ifdef SSA_DIAGNOSTICS
    std::cout << "unused val elim" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    if (!unusedCodeElim.performedWork) {
      break;
    }
  }

  ssa::RegAlloc allocator(cc, argRegs);
  builder.head.accept(allocator);
  Label notUsedLabel;
  ssa::Emitter emitter(cc, funcs, funcLabel, decl.name, error, notUsedLabel);
  builder.head.accept(emitter);
#ifdef SSA_DIAGNOSTICS
  std::cout << "final code" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif
}

void vaiven::generateTypeShapeProlog(FuncDecl<>& decl, FunctionUsage* usage, asmjit::X86Compiler& cc, asmjit::Label* optimizeLabel, vector<X86Gp>& argRegs) {
  *optimizeLabel = cc.newLabel();
  X86Gp count = cc.newInt32();
  cc.mov(count, asmjit::x86::dword_ptr((uint64_t) &usage->count));
  cc.add(count, 1);
  cc.mov(asmjit::x86::dword_ptr((uint64_t) &usage->count), count);
  cc.cmp(count, HOT_COUNT);
  cc.je(*optimizeLabel);

  X86Gp checkReg = cc.newUInt64();
  X86Gp orReg = cc.newUInt64();
  for (int i = 0; i < decl.args.size(); ++i) {
    usage->argShapes[i].raw = 0; // initialize
    X86Gp arg = argRegs[i];

    Label afterCheck = cc.newLabel();
    Label noPointerCheck = cc.newLabel();

    cc.mov(checkReg, MAX_PTR);
    cc.cmp(arg, checkReg);
    // can't derefence if it isn't a pointer
    // use jae because its unsigned
    cc.jae(noPointerCheck);
    // now we can dereference it
    cc.mov(orReg, x86::ptr(arg));
    cc.shl(orReg, POINTER_TAG_SHIFT);
    cc.jmp(afterCheck);

    cc.bind(noPointerCheck);

    cc.mov(checkReg, MIN_DBL);
    cc.mov(orReg, DOUBLE_SHAPE);
    cc.cmp(arg, checkReg);
    // use jae because its unsigned
    cc.jae(afterCheck);

    cc.mov(orReg, arg);
    cc.shr(orReg, PRIMITIVE_TAG_SHIFT);
    // creates a tag for ints, bools, and void

    cc.bind(afterCheck);
    cc.mov(checkReg.r16(), x86::word_ptr((uint64_t) &usage->argShapes[i]));
    cc.or_(checkReg.r16(), orReg.r16());
    cc.mov(x86::word_ptr((uint64_t) &usage->argShapes[i]), checkReg.r16());
  }
}

void vaiven::generateOptimizeEpilog(FuncDecl<>& decl, FuncSignature& sig, asmjit::X86Compiler& cc, Label& optimizeLabel, vector<X86Gp>& argRegs, Functions& funcs) {
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

  CCFuncCall* optimizedCall = cc.call(optimizedAddr, sig);
  for (int i = 0; i < decl.args.size(); ++i) {
    optimizedCall->setArg(i, argRegs[i]);
  }
  X86Gp optimizedRet = cc.newUInt64();
  optimizedCall->setRet(0, optimizedRet);
  
  cc.ret(optimizedRet);
}
