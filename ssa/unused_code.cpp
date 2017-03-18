#include "unused_code.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void UnusedCodeEliminator::remove(Instruction* instr) {
  if (lastInstr != NULL) {
    lastInstr->next = instr->next;
    instr->next = NULL;
    delete instr;
  } else {
    Instruction* next = instr->next;
    instr->next = NULL;
    curBlock->head.reset(next);
  }
  performedWork = true;
}

void UnusedCodeEliminator::visitPureInstr(Instruction& instr) {
  // save next before instr is maybe freed!!
  Instruction* next = instr.next;
  if (instr.usages.size() == 0) {
     remove(&instr);
  }
}

void UnusedCodeEliminator::visitPhiInstr(PhiInstr& instr) {
  vector<Instruction* >::iterator it = instr.inputs.begin();
  // phi nodes involving dead blocks get NULL inputs that need to be deleted
  while (it != instr.inputs.end()) {
    if (*it == NULL) {
      it = instr.inputs.erase(it);
    } else {
      ++it;
    }
  }

  // phi(x) is the same as just x
  if (instr.inputs.size() == 1) {
    instr.replaceUsagesWith(instr.inputs[0]);
  }

  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitArgInstr(ArgInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitConstantInstr(ConstantInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCallInstr(CallInstr& instr) {
  if (instr.func.isPure) {
    visitPureInstr(instr);
  }
}

void UnusedCodeEliminator::visitTypecheckInstr(TypecheckInstr& instr) {
  // TODO should really be somewhere else
  if (instr.inputs[0]->type == instr.type) {
    instr.replaceUsagesWith(instr.inputs[0]);
    remove(&instr);
  }
}

void UnusedCodeEliminator::visitBoxInstr(BoxInstr& instr) {
  // this happens for dead phis sometimes
  if (instr.inputs[0] == NULL) {
    remove(&instr);
  } else {
    visitPureInstr(instr);
  }
}

void UnusedCodeEliminator::visitAddInstr(AddInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitIntAddInstr(IntAddInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitStrAddInstr(StrAddInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitSubInstr(SubInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitMulInstr(MulInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitDivInstr(DivInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitNotInstr(NotInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpEqInstr(CmpEqInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpIneqInstr(CmpIneqInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpGtInstr(CmpGtInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpGteInstr(CmpGteInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpLtInstr(CmpLtInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitCmpLteInstr(CmpLteInstr& instr) {
  visitPureInstr(instr);
}

void UnusedCodeEliminator::visitErrInstr(ErrInstr& instr) {
  delete instr.next;
  instr.next = NULL;
}

void UnusedCodeEliminator::visitRetInstr(RetInstr& instr) {
  delete instr.next;
  instr.next = NULL;
}

void UnusedCodeEliminator::visitJmpCcInstr(JmpCcInstr& instr) {
  delete instr.next;
  instr.next = NULL;
}

void UnusedCodeEliminator::visitBlock(Block& block) {
  // entry point is always used
  if (allBlocks.size() == 0) {
    usedBlocks.insert(&block);
  }

  allBlocks.insert(&block);

  curBlock = &block;
  lastInstr = NULL;
  Instruction* next = block.head.get();
  while (next != NULL) {
    next->accept(*this);
    // special cases: next was deleted
    if (lastInstr != NULL && lastInstr->next != next) {
      next = lastInstr->next;
    } else if (lastInstr == NULL && block.head.get() != next) {
      next = block.head.get();
    } else {
      lastInstr = next;
      next = next->next;
    }
  }

  if (block.exits.size() && lastInstr != NULL && (lastInstr->tag == INSTR_RET || lastInstr->tag == INSTR_ERR)) {
    // all exits are dead code
    block.exits.clear();
    performedWork = true;
  }

  vector<unique_ptr<BlockExit>>::iterator it = block.exits.begin();
  while (it != block.exits.end()) {
    if ((*it)->tag == BLOCK_EXIT_UNCONDITIONAL) {
      (*it)->accept(*this);
      usedBlocks.insert((*it)->toGoTo);
      ++it;

      if (it != block.exits.end()) {
        block.exits.erase(it, block.exits.end());
        performedWork = true;
      }
      break;
    } else {
      ConditionalBlockExit& exit = static_cast<ConditionalBlockExit&>(**it);

      // find dead or guaranteed jmps
      if (exit.condition->tag == INSTR_JMPCC
          && exit.condition->inputs[0]->tag == INSTR_CONSTANT) {
        // we know its an int, otherwise it'd be a type error
        bool alwaysJmp = static_cast<ConstantInstr*>(exit.condition->inputs[0])->val.getBool();

        if (alwaysJmp) {
          usedBlocks.insert((*it)->toGoTo);
          BlockExit* unconditionalExit = new UnconditionalBlockExit((*it)->toGoTo);
          it->reset(unconditionalExit);
          ++it;
          block.exits.erase(it, block.exits.end());

          performedWork = true;
          break;
        } else {
          // never jmp
          it = block.exits.erase(it);

          performedWork = true;
          continue;
        }
      }

      // live and unguaranteed jmps
      (*it)->accept(*this);
      usedBlocks.insert((*it)->toGoTo);
      ++it;
    }
  }

  if (block.next != NULL) {
    backRefs[&*block.next] = &block;
    block.next->accept(*this);
  } else {
    // we're at the end, clean up unused blocks
    for (set<Block*>::iterator it = allBlocks.begin(); it != allBlocks.end(); ++it) {
      if (usedBlocks.find(*it) != usedBlocks.end()) {
        continue;
      }

      Block* unusedBlock = *it;
      Block* priorBlock = backRefs[unusedBlock];
      Block* nextBlock = &*unusedBlock->next;

      priorBlock->next.reset(unusedBlock->next.release());
      backRefs[nextBlock] = priorBlock;

      performedWork = true;
    }
  }
}

void UnusedCodeEliminator::visitConditionalBlockExit(ConditionalBlockExit& exit) {
  // the instruction in here is never dead
}
