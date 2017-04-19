#include "common_subexpression.h"

using namespace vaiven::ssa;

bool CommonSubexpressionEliminator::visitFoldableInstruction(Instruction& instr) {
  auto pair = make_pair(instr.tag, instr.inputs);
  auto search = prevInstructions.find(make_pair(instr.tag, instr.inputs));
  if (search == prevInstructions.end()) {
    prevInstructions[pair] = vector<Instruction*>();
    prevInstructions[pair].push_back(&instr);
    return false;
  }

  vector<Instruction*>& instructions = search->second;
  Instruction* dominatingEquivalent = NULL;
  for (vector<Instruction*>::iterator it = instructions.begin();
      it != instructions.end();
      ++it) {
    Block* block = (*it)->block;
    set<Block*> curDominators = curBlock->dominators;

    if (block == curBlock || curDominators.find(block) != curDominators.end()) {
      dominatingEquivalent = *it;
      break;
    }
  }

  // TODO move instructions up into shared dominators
  if (dominatingEquivalent == NULL) {
    return false;
  }

  for (set<Instruction*>::iterator it = instr.usages.begin();
      it != instr.usages.end();
      ++it) {
    if ((*it)->tag == INSTR_PHI) {
      // can't move values used by PHIs trivially
      return false;
    }
  }

  instr.replaceUsagesWith(dominatingEquivalent);

  performedWork = true;
  return true;
}

void CommonSubexpressionEliminator::visitPhiInstr(PhiInstr& instr) {
}

void CommonSubexpressionEliminator::visitArgInstr(ArgInstr& instr) {
}

void CommonSubexpressionEliminator::visitConstantInstr(ConstantInstr& instr) {
  // TODO reduce copying between this and nonconstant foldable instructions
  auto search = constants.find(instr.val);
  if (search == constants.end()) {
    constants[instr.val] = vector<Instruction*>();
    constants[instr.val].push_back(&instr);
    return;
  }

  vector<Instruction*>& instructions = search->second;
  Instruction* dominatingEquivalent = NULL;
  for (vector<Instruction*>::iterator it = instructions.begin();
      it != instructions.end();
      ++it) {
    Block* block = (*it)->block;
    set<Block*> curDominators = curBlock->dominators;

    if (block == curBlock || curDominators.find(block) != curDominators.end()) {
      dominatingEquivalent = *it;
      break;
    }
  }

  // TODO move instructions up into shared dominators
  if (dominatingEquivalent == NULL) {
    return;
  }

  for (set<Instruction*>::iterator it = instr.usages.begin();
      it != instr.usages.end();
      ++it) {
    if ((*it)->tag == INSTR_PHI) {
      // can't move values used by PHIs trivially
      return;
    }
  }

  instr.replaceUsagesWith(dominatingEquivalent);

  performedWork = true;
}

void CommonSubexpressionEliminator::visitCallInstr(CallInstr& instr) {
  if (instr.func.isNative && instr.func.isPure) {
    visitFoldableInstruction(instr);
  }
}

void CommonSubexpressionEliminator::visitTypecheckInstr(TypecheckInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitBoxInstr(BoxInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitUnboxInstr(UnboxInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitToDoubleInstr(ToDoubleInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntToDoubleInstr(IntToDoubleInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitAddInstr(AddInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitStrAddInstr(StrAddInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitIntAddInstr(IntAddInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleAddInstr(DoubleAddInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitSubInstr(SubInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntSubInstr(IntSubInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleSubInstr(DoubleSubInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitMulInstr(MulInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntMulInstr(IntMulInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleMulInstr(DoubleMulInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDivInstr(DivInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitNotInstr(NotInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitCmpEqInstr(CmpEqInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntCmpEqInstr(IntCmpEqInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntCmpIneqInstr(IntCmpIneqInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitCmpGtInstr(CmpGtInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntCmpGtInstr(IntCmpGtInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitCmpGteInstr(CmpGteInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntCmpGteInstr(IntCmpGteInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitCmpLtInstr(CmpLtInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntCmpLtInstr(IntCmpLtInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitCmpLteInstr(CmpLteInstr& instr) {
  if (visitFoldableInstruction(instr)) {
    instr.safelyDeletable = true;
  }
}

void CommonSubexpressionEliminator::visitIntCmpLteInstr(IntCmpLteInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr) {
  visitFoldableInstruction(instr);
}

void CommonSubexpressionEliminator::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
}

void CommonSubexpressionEliminator::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
}

void CommonSubexpressionEliminator::visitListAccessInstr(ListAccessInstr& instr) {
}

void CommonSubexpressionEliminator::visitListStoreInstr(ListStoreInstr& instr) {
}

void CommonSubexpressionEliminator::visitListInitInstr(ListInitInstr& instr) {
}

void CommonSubexpressionEliminator::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
}

void CommonSubexpressionEliminator::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
}

void CommonSubexpressionEliminator::visitObjectAccessInstr(ObjectAccessInstr& instr) {
}

void CommonSubexpressionEliminator::visitObjectStoreInstr(ObjectStoreInstr& instr) {
}

void CommonSubexpressionEliminator::visitErrInstr(ErrInstr& instr) {
}

void CommonSubexpressionEliminator::visitRetInstr(RetInstr& instr) {
}

void CommonSubexpressionEliminator::visitJmpCcInstr(JmpCcInstr& instr) {
}
