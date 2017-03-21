#ifndef VAIVEN_VISITOR_HEADER_SSA_FORWARD_VISITOR
#define VAIVEN_VISITOR_HEADER_SSA_FORWARD_VISITOR
#include "cfg.h"

namespace vaiven { namespace ssa {

class ForwardVisitor : public SsaVisitor {

  public:
  ForwardVisitor() : curBlock(NULL), lastInstr(NULL) {};

  virtual void visitPhiInstr(PhiInstr& instr)=0;
  virtual void visitArgInstr(ArgInstr& instr)=0;
  virtual void visitConstantInstr(ConstantInstr& instr)=0;
  virtual void visitCallInstr(CallInstr& instr)=0;
  virtual void visitTypecheckInstr(TypecheckInstr& instr)=0;
  virtual void visitBoxInstr(BoxInstr& instr)=0;
  virtual void visitAddInstr(AddInstr& instr)=0;
  virtual void visitIntAddInstr(IntAddInstr& instr)=0;
  virtual void visitStrAddInstr(StrAddInstr& instr)=0;
  virtual void visitSubInstr(SubInstr& instr)=0;
  virtual void visitMulInstr(MulInstr& instr)=0;
  virtual void visitDivInstr(DivInstr& instr)=0;
  virtual void visitNotInstr(NotInstr& instr)=0;
  virtual void visitCmpEqInstr(CmpEqInstr& instr)=0;
  virtual void visitCmpIneqInstr(CmpIneqInstr& instr)=0;
  virtual void visitCmpGtInstr(CmpGtInstr& instr)=0;
  virtual void visitCmpGteInstr(CmpGteInstr& instr)=0;
  virtual void visitCmpLtInstr(CmpLtInstr& instr)=0;
  virtual void visitCmpLteInstr(CmpLteInstr& instr)=0;
  virtual void visitDynamicAccessInstr(DynamicAccessInstr& instr)=0;
  virtual void visitDynamicStoreInstr(DynamicStoreInstr& instr)=0;
  virtual void visitListAccessInstr(ListAccessInstr& instr)=0;
  virtual void visitListStoreInstr(ListStoreInstr& instr)=0;
  virtual void visitListInitInstr(ListInitInstr& instr)=0;
  virtual void visitErrInstr(ErrInstr& instr)=0;
  virtual void visitRetInstr(RetInstr& instr)=0;
  virtual void visitJmpCcInstr(JmpCcInstr& instr)=0;

  virtual void visitBlock(Block& block);
  virtual void visitUnconditionalBlockExit(UnconditionalBlockExit& exit);
  virtual void visitConditionalBlockExit(ConditionalBlockExit& exit);

  Block* curBlock;
  Instruction* lastInstr;
};

}}

#endif
