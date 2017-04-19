#ifndef VAIVEN_VISITOR_HEADER_SSA_TYPE_ANALYSIS
#define VAIVEN_VISITOR_HEADER_SSA_TYPE_ANALYSIS
#include "forward_visitor.h"

namespace vaiven { namespace ssa {

class TypeAnalysis : public ForwardVisitor {

  public:
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
  void visitErrInstr(ErrInstr& instr);
  void visitRetInstr(RetInstr& instr);
  void visitJmpCcInstr(JmpCcInstr& instr);

  void typecheckInput(Instruction& instr, VaivenStaticType expectedType, int input);
  void visitBinIntInstruction(Instruction& instr);
  void visitBinDoubleInstruction(Instruction& instr);
  void emit(Instruction* instr);
  void box(Instruction** input, Instruction* instr);

};

}}

#endif
