#include "inliner.h"
#include "function_merge.h"
#include "../visitor/ssa_builder.h"

#include <iostream>

using namespace vaiven::ssa;
using namespace vaiven::ast;

const int STOP_INLINING_AT_SIZE = 1024; // ?
const int MAX_INLINE_SIZE = 512; // ?
const int WORST_INSTRUCTION_SIZE = 15; // wowowow
const int BOX_COST = WORST_INSTRUCTION_SIZE * 2;
const int TYPECHECK_COST = WORST_INSTRUCTION_SIZE * 3;
const int SPILL_COST = WORST_INSTRUCTION_SIZE * 2;
const int FUNCTION_CALL_COST = WORST_INSTRUCTION_SIZE * 2;
const int ARG_COST = WORST_INSTRUCTION_SIZE * 5; // every arg is a chance to optimize

void Inliner::visitPhiInstr(PhiInstr& instr) {
}

void Inliner::visitArgInstr(ArgInstr& instr) {
}

void Inliner::visitConstantInstr(ConstantInstr& instr) {
}

void Inliner::visitCallInstr(CallInstr& instr) {
  Function& func = instr.func;
  if (func.isNative) {
    return;
  }

  // arbitrary max sizes to stop inlining
  if (currentWorstSize > STOP_INLINING_AT_SIZE
      || func.worstSize > MAX_INLINE_SIZE) {
#ifdef INLINING_DIAGNOSTICS
    std::cout << "not inlining " << instr.funcName << " because we've hit a limit" << std::endl;
#endif
    return;
  }

#ifdef INLINING_DIAGNOSTICS
  std::cout << "considering inlining " << instr.funcName << std::endl;
#endif

  int callOverheadGuess = FUNCTION_CALL_COST;

  int argc = instr.inputs.size();
  callOverheadGuess += argc * SPILL_COST + argc * ARG_COST;
  for (vector<Instruction*>::iterator it = instr.inputs.begin(); it != instr.inputs.end(); ++it) {
    if ((*it)->tag == INSTR_BOX) {
      callOverheadGuess += BOX_COST;
    } else if ((*it)->type != VAIVEN_STATIC_TYPE_UNKNOWN) {
      // assume passed in args are type checked at least once
      callOverheadGuess += TYPECHECK_COST;
    }
  }

  // rax will spill, must be a boxed value, may be typechecked after
  callOverheadGuess += SPILL_COST + BOX_COST;
  for (set<Instruction*>::iterator it = instr.usages.begin(); it != instr.usages.end(); ++it) {
    if ((*it)->tag == INSTR_TYPECHECK) {
      callOverheadGuess += TYPECHECK_COST;
    }
  }

  // assume 70% reduction in code size from hot optimization (?) vs worst size
  int afterOptimizeGuessSize = (func.worstSize - callOverheadGuess) * 3 / 10;

  // TODO count % of times executed rather than "is hot" (which doesn't even really matter)
  bool isHot = func.usage->count == HOT_COUNT;

#ifdef INLINING_DIAGNOSTICS
  std::cout << "estimated overhead of calling is " << callOverheadGuess << std::endl;
  std::cout << "estimated size of inlining is " << afterOptimizeGuessSize << std::endl;
  std::cout << instr.funcName << (isHot ? " is hot " : " is cold ") << std::endl;
#endif

  // if the function isn't smaller than its overhead, and its not hot, don't inline
  if (afterOptimizeGuessSize > callOverheadGuess && !isHot) {
#ifdef INLINING_DIAGNOSTICS
    std::cout << "will not inline" << std::endl;
#endif
    return;
  }

#ifdef INLINING_DIAGNOSTICS
  std::cout << "inlining will proceed" << std::endl;
#endif

  currentWorstSize += func.worstSize - callOverheadGuess;

  FuncDecl<>& funcDecl = *func.ast;

  Block* returnPoint = new Block();
  PhiInstr* resultPhi = new PhiInstr();

  // note: usage is unused here...
  visitor::SsaBuilder buildInlineCode(*func.usage, funcs);

  // TODO consolidate this better with SsaBuilder
  for (int i = 0; i < funcDecl.args.size(); ++i) {
    if (instr.inputs.size() < i) {
      ConstantInstr* void_ = new ConstantInstr(Value());
      buildInlineCode.emit(void_);
      buildInlineCode.scope.put(funcDecl.args[i], void_);
    } else {
      buildInlineCode.scope.put(funcDecl.args[i], instr.inputs[i]);
    }
  }

  for(vector<unique_ptr<Statement<> > >::iterator it = funcDecl.statements.begin();
      it != funcDecl.statements.end();
      ++it) {
    (*it)->accept(buildInlineCode);
  }

  // TODO proper logic here
  if (buildInlineCode.cur != NULL && buildInlineCode.cur->tag != INSTR_RET) {
    buildInlineCode.emit(new RetInstr(buildInlineCode.cur));
  }

  FunctionMerger merger(returnPoint, resultPhi);
  buildInlineCode.head.accept(merger);

  Block* finalBlock = merger.curBlock;

  resultPhi->next = instr.next;
  instr.next = NULL;
  instr.replaceUsagesWith(resultPhi);
  if (lastInstr != NULL) {
    lastInstr->next = buildInlineCode.head.head.release();
    delete &instr;
  } else {
    curBlock->head.reset(buildInlineCode.head.head.release());
  }
  std::swap(returnPoint->exits, curBlock->exits);
  returnPoint->next.reset(curBlock->next.release());
  finalBlock->next.reset(returnPoint);
  curBlock->next.reset(buildInlineCode.head.next.release());
  std::swap(curBlock->exits, buildInlineCode.head.exits);
  finalBlock->exits.push_back(unique_ptr<BlockExit>(new UnconditionalBlockExit(returnPoint)));

  returnPoint->head.reset(resultPhi);
}

void Inliner::visitTypecheckInstr(TypecheckInstr& instr) {
}

void Inliner::visitBoxInstr(BoxInstr& instr) {
}

void Inliner::visitUnboxInstr(UnboxInstr& instr) {
}

void Inliner::visitToDoubleInstr(ToDoubleInstr& instr) {
}

void Inliner::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
}

void Inliner::visitAddInstr(AddInstr& instr) {
}

void Inliner::visitStrAddInstr(StrAddInstr& instr) {
}

void Inliner::visitIntAddInstr(IntAddInstr& instr) {
}

void Inliner::visitDoubleAddInstr(DoubleAddInstr& instr) {
}

void Inliner::visitSubInstr(SubInstr& instr) {
}

void Inliner::visitIntSubInstr(IntSubInstr& instr) {
}

void Inliner::visitDoubleSubInstr(DoubleSubInstr& instr) {
}

void Inliner::visitMulInstr(MulInstr& instr) {
}

void Inliner::visitIntMulInstr(IntMulInstr& instr) {
}

void Inliner::visitDoubleMulInstr(DoubleMulInstr& instr) {
}

void Inliner::visitDivInstr(DivInstr& instr) {
}

void Inliner::visitNotInstr(NotInstr& instr) {
}

void Inliner::visitCmpEqInstr(CmpEqInstr& instr) {
}

void Inliner::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
}

void Inliner::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
}

void Inliner::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void Inliner::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
}

void Inliner::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
}

void Inliner::visitCmpGtInstr(CmpGtInstr& instr) {
}

void Inliner::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
}

void Inliner::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
}

void Inliner::visitCmpGteInstr(CmpGteInstr& instr) {
}

void Inliner::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
}

void Inliner::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
}

void Inliner::visitCmpLtInstr(CmpLtInstr& instr) {
}

void Inliner::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
}

void Inliner::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
}

void Inliner::visitCmpLteInstr(CmpLteInstr& instr) {
}

void Inliner::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
}

void Inliner::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
}

void Inliner::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
}

void Inliner::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
}

void Inliner::visitListAccessInstr(ListAccessInstr& instr) {
}

void Inliner::visitListStoreInstr(ListStoreInstr& instr) {
}

void Inliner::visitListInitInstr(ListInitInstr& instr) {
}

void Inliner::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
}

void Inliner::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
}

void Inliner::visitObjectAccessInstr(ObjectAccessInstr& instr) {
}

void Inliner::visitObjectStoreInstr(ObjectStoreInstr& instr) {
}

void Inliner::visitErrInstr(ErrInstr& instr) {
}

void Inliner::visitRetInstr(RetInstr& instr) {
}

void Inliner::visitJmpCcInstr(JmpCcInstr& instr) {
}

void Inliner::visitBlock(Block& block) {
  curBlock = &block;
  lastInstr = NULL;
  Instruction* next = block.head.get();
  while (next != NULL) {
    next->accept(*this);
    // special cases: next was replaced
    if (lastInstr != NULL && lastInstr->next != next) {
      next = lastInstr->next;
    } else if (lastInstr == NULL && block.head.get() != next) {
      next = block.head.get();
    } else {
      lastInstr = next;
      next = next->next;
    }
  }

  if (block.next != NULL) {
    block.next->accept(*this);
  }
}
