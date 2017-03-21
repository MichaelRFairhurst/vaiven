#ifndef VAIVEN_VISITOR_HEADER_SSA_EMITTER
#define VAIVEN_VISITOR_HEADER_SSA_EMITTER
#include "forward_visitor.h"
#include "../functions.h"
#include "../error_compiler.h"

namespace vaiven { namespace ssa {

class Emitter : public ForwardVisitor {

  public:
  Emitter(asmjit::X86Compiler& cc, Functions& funcs, asmjit::Label funcLabel, string funcName, ErrorCompiler& error, asmjit::Label deoptimizeLabel)
      : cc(cc), funcs(funcs), funcLabel(funcLabel), funcName(funcName), error(error), deoptimizeLabel(deoptimizeLabel) {};

  void visitPhiInstr(PhiInstr& instr);
  void visitArgInstr(ArgInstr& instr);
  void visitConstantInstr(ConstantInstr& instr);
  void visitCallInstr(CallInstr& instr);
  void visitTypecheckInstr(TypecheckInstr& instr);
  void visitBoxInstr(BoxInstr& instr);
  void visitAddInstr(AddInstr& instr);
  void visitIntAddInstr(IntAddInstr& instr);
  void visitStrAddInstr(StrAddInstr& instr);
  void visitSubInstr(SubInstr& instr);
  void visitMulInstr(MulInstr& instr);
  void visitDivInstr(DivInstr& instr);
  void visitNotInstr(NotInstr& instr);
  void visitCmpEqInstr(CmpEqInstr& instr);
  void visitCmpIneqInstr(CmpIneqInstr& instr);
  void visitCmpGtInstr(CmpGtInstr& instr);
  void visitCmpGteInstr(CmpGteInstr& instr);
  void visitCmpLtInstr(CmpLtInstr& instr);
  void visitCmpLteInstr(CmpLteInstr& instr);
  void visitDynamicAccessInstr(DynamicAccessInstr& instr);
  void visitDynamicStoreInstr(DynamicStoreInstr& instr);
  void visitListAccessInstr(ListAccessInstr& instr);
  void visitListStoreInstr(ListStoreInstr& instr);
  void visitListInitInstr(ListInitInstr& instr);
  void doCmpEqInstr(CmpEqInstr& instr);
  void doCmpIneqInstr(CmpIneqInstr& instr);
  void doCmpGtInstr(CmpGtInstr& instr);
  void doCmpGteInstr(CmpGteInstr& instr);
  void doCmpLtInstr(CmpLtInstr& instr);
  void doCmpLteInstr(CmpLteInstr& instr);
  void visitErrInstr(ErrInstr& instr);
  void visitRetInstr(RetInstr& instr);
  void visitJmpCcInstr(JmpCcInstr& instr);

  void visitUnconditionalBlockExit(UnconditionalBlockExit& exit);
  void visitConditionalBlockExit(ConditionalBlockExit& exit);
  void visitBlock(Block& block);

  ErrorCompiler& error;
  asmjit::X86Compiler& cc;
  asmjit::Label funcLabel;
  asmjit::Label deoptimizeLabel;
  string funcName;
  Functions& funcs;
};

}}

#endif
