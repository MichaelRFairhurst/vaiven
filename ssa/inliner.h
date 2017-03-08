#ifndef VAIVEN_VISITOR_HEADER_SSA_INLINER
#define VAIVEN_VISITOR_HEADER_SSA_INLINER

#include "forward_visitor.h"
#include "../functions.h"
#include <map>

namespace vaiven { namespace ssa {

class Inliner : public ForwardVisitor {
  public:
  Inliner(Functions& funcs) : funcs(funcs), inlineCount(0) {};

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
  void visitJmpCcInstr(JmpCcInstr& instr);
  void visitBlock(Block& block);

  Functions& funcs;
  int inlineCount;
};

}}

#endif
