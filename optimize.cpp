#include "optimize.h"

#include "stdio.h"
#include <iostream>

#include "visitor/usage_applier.h"
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
using namespace vaiven::ast;

using namespace vaiven;
using namespace asmjit;

// Error handler that just prints the error and lets AsmJit ignore it.
class PrintErrorHandler2 : public asmjit::ErrorHandler {
public:
  // Return `true` to set last error to `err`, return `false` to do nothing.
  bool handleError(asmjit::Error err, const char* message, asmjit::CodeEmitter* origin) override {
    fprintf(stderr, "Error: %s\n", message);
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

  vaiven::visitor::UsageApplier applier(*funcs.funcs[funcDecl.name]->usage);
  funcDecl.accept(applier);
  
  performOptimize(funcDecl, funcs, *funcs.funcs[funcDecl.name]->usage);

#ifdef OPTIMIZATION_DIAGNOSTICS
  std::cout << "done optimizing" << std::endl;
#endif
  return funcs.funcs[funcDecl.name]->fptr;
}

void vaiven::performOptimize(ast::FuncDecl<TypedLocationInfo>& decl, Functions& funcs, FunctionUsage& usageInfo) {
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

  uint8_t sigArgs[decl.args.size()];

  for (int i = 0; i < decl.args.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, decl.args.size());
  CCFunc* curFunc = cc.addFunc(sig);
  curFunc->getFrameInfo().addAttributes(asmjit::FuncFrameInfo::kAttrPreserveFP);

  vector<X86Gp> argRegs;
  Label deoptimizeLabel = cc.newLabel();
  for (int i = 0; i < decl.args.size(); ++i) {
    X86Gp arg = cc.newInt64();
    cc.setArg(i, arg);
    argRegs.push_back(arg);
  }

  SsaBuilder builder(usageInfo, funcs);
  decl.accept(builder);
#ifdef SSA_DIAGNOSTICS
  ssa::PrintVisitor printer;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  ssa::Inliner inliner(funcs, funcs.funcs[decl.name]->worstSize);
  builder.head.accept(inliner);
#ifdef SSA_DIAGNOSTICS
  std::cout << "inliner" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  ssa::DominatorBuilder dbuilder;
  dbuilder.firstBuild(builder.head);
#ifdef SSA_DIAGNOSTICS
  std::cout << "dominator builder" << std::endl;
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

    ssa::InstructionCombiner instrComb;
    builder.head.accept(instrComb);
#ifdef SSA_DIAGNOSTICS
    std::cout << "instr comb" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    ssa::LoopInvariantCodeMover loopInvariant;
    builder.head.accept(loopInvariant);
    if (loopInvariant.requiresRebuildDominators) {
      dbuilder.reset(builder.head);
    }
#ifdef SSA_DIAGNOSTICS
    std::cout << "licm" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    ssa::CommonSubexpressionEliminator commonSubElim;
    builder.head.accept(commonSubElim);
#ifdef SSA_DIAGNOSTICS
    std::cout << "cse" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    ssa::UnusedCodeEliminator unusedCodeElim;
    builder.head.accept(unusedCodeElim);
    if (unusedCodeElim.requiresRebuildDominators) {
      dbuilder.reset(builder.head);
    }
#ifdef SSA_DIAGNOSTICS
    std::cout << "unused val elim" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    if (!unusedCodeElim.performedWork && !instrComb.performedWork && !constantProp.performedWork && !loopInvariant.performedWork && !commonSubElim.performedWork) {
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
  if (jmpThreader.requiresRebuildDominators) {
    dbuilder.reset(builder.head);
  }
#ifdef SSA_DIAGNOSTICS
  std::cout << "jmp threading" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  while(true) {
    ssa::UnusedCodeEliminator unusedCodeElim;
    builder.head.accept(unusedCodeElim);
    if (unusedCodeElim.requiresRebuildDominators) {
      dbuilder.reset(builder.head);
    }
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
  ErrorCompiler error(cc);
  ssa::Emitter emitter(cc, funcs, curFunc->getLabel(), decl.name, error, deoptimizeLabel);
  builder.head.accept(emitter);
#ifdef SSA_DIAGNOSTICS
  std::cout << "final code" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  cc.bind(deoptimizeLabel);
  CCFuncCall* deoptimizedCall = cc.call((unsigned long long) funcs.funcs[decl.name]->fptr, sig);
  for (int i = 0; i < decl.args.size(); ++i) {
    deoptimizedCall->setArg(i, argRegs[i]);
  }
  X86Gp deoptimizedRet = cc.newUInt64();
  deoptimizedCall->setRet(0, deoptimizedRet);
  
  cc.ret(deoptimizedRet);

  error.generateTypeErrorEpilog();
  cc.endFunc();
  cc.finalize();


  OverkillFunc fptr;
  funcs.runtime.add(&fptr, &codeHolder);
  funcs.funcs[decl.name]->slowfptr = funcs.funcs[decl.name]->fptr;
  funcs.funcs[decl.name]->fptr = fptr;
}
