#ifndef VAIVEN_VISITOR_HEADER_SSA_USAGES_BUILDER
#define VAIVEN_VISITOR_HEADER_SSA_USAGES_BUILDER
#include "ssa.h"

namespace vaiven { namespace ssa {

class UnusedCodeEliminator : public SsaVisitor {

  public:
  UnusedCodeEliminator() : performedWork(false), start(NULL), last(NULL) {};

  void visitPhiInstr(PhiInstr& instr);
  void visitArgInstr(ArgInstr& instr);
  void visitConstantInstr(ConstantInstr& instr);
  void visitCallInstr(CallInstr& instr);
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
  void visitDynamicAccessInstr(DynamicAccessInstr& instr);
  void visitDynamicStoreInstr(DynamicStoreInstr& instr);
  void visitListAccessInstr(ListAccessInstr& instr);
  void visitListStoreInstr(ListStoreInstr& instr);
  void visitListInitInstr(ListInitInstr& instr);
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
