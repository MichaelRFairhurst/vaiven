#ifndef VAIVEN_VISITOR_HEADER_SSA_UNUSED_CODE
#define VAIVEN_VISITOR_HEADER_SSA_UNUSED_CODE
#include "ssa.h"

namespace vaiven { namespace ssa {

class UnusedCodeEliminator : public SsaVisitor {

  public:
  UnusedCodeEliminator() : performedWork(false), start(NULL), last(NULL) {};

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

  bool performedWork;
  Instruction* start;
  Instruction* last;

  void visitPureInstr(Instruction& instr);
  void visitImpureInstr(Instruction& instr);
  void remove(Instruction* instr);
};

}}

#endif
