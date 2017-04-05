#ifndef VAIVEN_VISITOR_HEADER_SSA_LOOP_INVARIANT
#define VAIVEN_VISITOR_HEADER_SSA_LOOP_INVARIANT

#include "forward_visitor.h"
#include <map>

namespace vaiven { namespace ssa {

class LoopInvariantCodeMover : public ForwardVisitor {
  public:
  LoopInvariantCodeMover() : requiresRebuildDominators(false), performedWork(false) {};

  void visitPureInstruction(Instruction& instr);

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
  void visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr);
  void visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr);
  void visitObjectAccessInstr(ObjectAccessInstr& instr);
  void visitObjectStoreInstr(ObjectStoreInstr& instr);
  void visitErrInstr(ErrInstr& instr);
  void visitRetInstr(RetInstr& instr);
  void visitJmpCcInstr(JmpCcInstr& instr);
  void visitBlock(Block& block);

  Block* currentPreHeader;
  Instruction* writePoint;
  bool requiresRebuildDominators;
  bool performedWork;
};

}}

#endif