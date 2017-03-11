#include "recompiler.h"

#include "../ast/all.h"
#include "../value.h"
#include "../runtime_error.h"
#include "ssa_builder.h"
#include "../ssa/emitter.h"
#include "../ssa/reg_alloc.h"
#include "../ssa/print_visitor.h"
#include "../ssa/constant_propagation.h"
#include "../ssa/instruction_combiner.h"
#include "../ssa/unused_code.h"
#include "../ssa/type_analysis.h"
#include "../ssa/constant_inliner.h"
#include "../ssa/jmp_threader.h"
#include "../ssa/inliner.h"

#include <iostream>
#include <stdint.h>

using namespace asmjit;
using namespace vaiven::visitor;

void ReCompiler::visitFuncDecl(FuncDecl<TypedLocationInfo>& decl) {
  uint8_t sigArgs[decl.args.size()];

  for (int i = 0; i < decl.args.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, decl.args.size());
  curFunc = cc.addFunc(sig);
  curFuncName = decl.name;

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

  ssa::TypeAnalysis typeAnalysis;
  builder.head.accept(typeAnalysis);
#ifdef SSA_DIAGNOSTICS
  std::cout << "type analysis" << std::endl;
  builder.head.accept(printer); printer.varIds.clear();
#endif

  while(true) {
    ssa::ConstantPropagator constant_prop;
    builder.head.accept(constant_prop);
#ifdef SSA_DIAGNOSTICS
    std::cout << "constant prop" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    ssa::InstructionCombiner instr_comb;
    builder.head.accept(instr_comb);
#ifdef SSA_DIAGNOSTICS
    std::cout << "instr comb" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    ssa::UnusedCodeEliminator unused_code_elim;
    builder.head.accept(unused_code_elim);
#ifdef SSA_DIAGNOSTICS
    std::cout << "unused val elim" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    if (!unused_code_elim.performedWork && !instr_comb.performedWork && !constant_prop.performedWork) {
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
    ssa::UnusedCodeEliminator unused_code_elim;
    builder.head.accept(unused_code_elim);
#ifdef SSA_DIAGNOSTICS
    std::cout << "unused val elim" << std::endl;
    builder.head.accept(printer); printer.varIds.clear();
#endif

    if (!unused_code_elim.performedWork) {
      break;
    }
  }
  ssa::RegAlloc allocator(cc, argRegs);
  builder.head.accept(allocator);
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

  error.generateTypeErrorProlog();
  cc.endFunc();
  cc.finalize();


  OverkillFunc fptr;
  funcs.runtime.add(&fptr, &codeHolder);
  funcs.funcs[decl.name]->slowfptr = funcs.funcs[decl.name]->fptr;
  funcs.funcs[decl.name]->fptr = fptr;
}
