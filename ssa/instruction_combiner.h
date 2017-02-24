#ifndef VAIVEN_VISITOR_HEADER_SSA_INSTRUCTION_COMBINER
#define VAIVEN_VISITOR_HEADER_SSA_INSTRUCTION_COMBINER
#include "ssa.h"

namespace vaiven { namespace ssa {

class InstructionCombiner : public SsaVisitor {

  public:
  InstructionCombiner() : performedWork(false) {};

  void visitPhiInstr(PhiInstr& instr);
  void visitArgInstr(ArgInstr& instr);
  void visitConstantInstr(ConstantInstr& instr);
  void visitCallInstr(CallInstr& instr);
  void visitTypecheckInstr(TypecheckInstr& instr);
  void visitBoxInstr(BoxInstr& instr);
  void visitAddInstr(AddInstr& instr);
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
  void visitErrInstr(ErrInstr& instr);
  void visitRetInstr(RetInstr& instr);

  bool isBinIntInstruction(Instruction& instr);
  void replaceReferencingNewConstant(Instruction& oldInstr, Instruction* newInstr);

  bool performedWork;
};

}}

#endif
