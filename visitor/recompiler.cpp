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

  typeErrorLabel = cc.newLabel();

  //Label deoptimizeLabel = cc.newLabel();
  //X86Gp checkArg = cc.newInt64();
  //for (int i = 0; i < decl.args.size(); ++i) {
  //  X86Gp arg = cc.newInt64();
  //  cc.setArg(i, arg);
  //  argRegs.push_back(arg);

  //  if (usageInfo.argShapes[i].isPureInt()) {
  //    cc.mov(checkArg, arg);
  //    cc.shr(checkArg, VALUE_TAG_SHIFT);
  //    cc.cmp(checkArg, INT_TAG_SHIFTED);
  //    cc.jne(deoptimizeLabel);
  //  }
  //}

  TypedLocationInfo endType;
  SsaBuilder builder(usageInfo);
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    //(*it)->accept(*this);
    
    endType = (*it)->resolvedData;
  }
  decl.accept(builder);
  ssa::PrintVisitor printer;
  builder.head.accept(printer); printer.varIds.clear();
  std::cout << "type analysis" << std::endl;
  ssa::TypeAnalysis typeAnalysis;
  builder.head.accept(typeAnalysis);
  builder.head.accept(printer); printer.varIds.clear();
  while(true) {
    std::cout << "constant prop" << std::endl;
    ssa::ConstantPropagator constant_prop;
    builder.head.accept(constant_prop);
    builder.head.accept(printer); printer.varIds.clear();

    std::cout << "instr comb" << std::endl;
    ssa::InstructionCombiner instr_comb;
    builder.head.accept(instr_comb);
    builder.head.accept(printer); printer.varIds.clear();

    std::cout << "unused val elim" << std::endl;
    ssa::UnusedCodeEliminator unused_code_elim;
    builder.head.accept(unused_code_elim);
    builder.head.accept(printer); printer.varIds.clear();

    if (!unused_code_elim.performedWork && !instr_comb.performedWork && !constant_prop.performedWork) {
      break;
    }

  }

  std::cout << "const inliner" << std::endl;
  ssa::ConstantInliner constInliner;
  builder.head.accept(constInliner);
  builder.head.accept(printer); printer.varIds.clear();

  std::cout << "jmp threading" << std::endl;
  ssa::JmpThreader jmpThreader;
  builder.head.accept(jmpThreader);
  builder.head.accept(printer); printer.varIds.clear();

  while(true) {
    std::cout << "unused val elim" << std::endl;
    ssa::UnusedCodeEliminator unused_code_elim;
    builder.head.accept(unused_code_elim);
    builder.head.accept(printer); printer.varIds.clear();

    if (!unused_code_elim.performedWork) {
      break;
    }
  }
  std::cout << "final code" << std::endl;
  ssa::RegAlloc allocator(cc);
  builder.head.accept(allocator);
  ssa::Emitter emitter(cc, funcs);
  builder.head.accept(emitter);
  builder.head.accept(printer); printer.varIds.clear();

  //if (vRegs.size()) {
  //  box(vRegs.top(), endType);
  //  cc.ret(vRegs.top());
  //} else {
  //  X86Gp voidReg = cc.newInt64();
  //  cc.mov(voidReg, Value().getRaw());
  //  cc.ret(voidReg);
  //}
  //
  //cc.bind(deoptimizeLabel);
  generateTypeErrorProlog();
  cc.endFunc();
  cc.finalize();


  OverkillFunc fptr;
  funcs.runtime.add(&fptr, &codeHolder);
  funcs.funcs[decl.name]->slowfptr = funcs.funcs[decl.name]->fptr;
  funcs.funcs[decl.name]->fptr = fptr;
}
