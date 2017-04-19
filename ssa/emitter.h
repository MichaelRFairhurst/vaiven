#ifndef VAIVEN_VISITOR_HEADER_SSA_EMITTER
#define VAIVEN_VISITOR_HEADER_SSA_EMITTER
#include "forward_visitor.h"
#include "../functions.h"
#include "../error_compiler.h"

namespace vaiven { namespace ssa {

class Emitter : public ForwardVisitor {

  public:
  Emitter(asmjit::X86Compiler& cc, Functions& funcs, asmjit::Label funcLabel, string funcName, ErrorCompiler& error, asmjit::Label deoptimizeLabel)
      : cc(cc), funcs(funcs), funcLabel(funcLabel), funcName(funcName), error(error), deoptimizeLabel(deoptimizeLabel), afterGuardsLabel(funcLabel) {};

  void visitPhiInstr(PhiInstr& instr);
  void visitArgInstr(ArgInstr& instr);
  void visitConstantInstr(ConstantInstr& instr);
  void visitCallInstr(CallInstr& instr);
  void visitTypecheckInstr(TypecheckInstr& instr);
  void visitBoxInstr(BoxInstr& instr);
  void visitUnboxInstr(UnboxInstr& instr);
  void visitToDoubleInstr(ToDoubleInstr& instr);
  void visitIntToDoubleInstr(IntToDoubleInstr& instr);
  void visitAddInstr(AddInstr& instr);
  void visitIntAddInstr(IntAddInstr& instr);
  void visitDoubleAddInstr(DoubleAddInstr& instr);
  void visitStrAddInstr(StrAddInstr& instr);
  void visitSubInstr(SubInstr& instr);
  void visitIntSubInstr(IntSubInstr& instr);
  void visitDoubleSubInstr(DoubleSubInstr& instr);
  void visitMulInstr(MulInstr& instr);
  void visitIntMulInstr(IntMulInstr& instr);
  void visitDoubleMulInstr(DoubleMulInstr& instr);
  void visitDivInstr(DivInstr& instr);
  void visitNotInstr(NotInstr& instr);
  void visitCmpEqInstr(CmpEqInstr& instr);
  void visitIntCmpEqInstr(IntCmpEqInstr& instr);
  void visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr);
  void visitCmpIneqInstr(CmpIneqInstr& instr);
  void visitIntCmpIneqInstr(IntCmpIneqInstr& instr);
  void visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr);
  void visitCmpGtInstr(CmpGtInstr& instr);
  void visitIntCmpGtInstr(IntCmpGtInstr& instr);
  void visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr);
  void visitCmpGteInstr(CmpGteInstr& instr);
  void visitIntCmpGteInstr(IntCmpGteInstr& instr);
  void visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr);
  void visitCmpLtInstr(CmpLtInstr& instr);
  void visitIntCmpLtInstr(IntCmpLtInstr& instr);
  void visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr);
  void visitCmpLteInstr(CmpLteInstr& instr);
  void visitIntCmpLteInstr(IntCmpLteInstr& instr);
  void visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr);
  void visitDynamicAccessInstr(DynamicAccessInstr& instr);
  void visitDynamicStoreInstr(DynamicStoreInstr& instr);
  void visitListAccessInstr(ListAccessInstr& instr);
  void visitListStoreInstr(ListStoreInstr& instr);
  void visitListInitInstr(ListInitInstr& instr);
  void visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr);
  void visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr);
  void visitObjectAccessInstr(ObjectAccessInstr& instr);
  void visitObjectStoreInstr(ObjectStoreInstr& instr);
  void doDoubleCmpInstr(Instruction& instr);
  void doIntCmpEqInstr(IntCmpEqInstr& instr);
  void doIntCmpIneqInstr(IntCmpIneqInstr& instr);
  void doIntCmpGtInstr(IntCmpGtInstr& instr);
  void doIntCmpGteInstr(IntCmpGteInstr& instr);
  void doIntCmpLtInstr(IntCmpLtInstr& instr);
  void doIntCmpLteInstr(IntCmpLteInstr& instr);
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
  asmjit::Label afterGuardsLabel;
  vector<ArgInstr*> args;
  string funcName;
  Functions& funcs;
};

}}

#endif
