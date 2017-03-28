#include "ssa.h"

using namespace vaiven::ssa;

void Instruction::replaceInput(Instruction* oldInstr, Instruction* newInstr) {
  for (vector<Instruction*>::iterator it = inputs.begin();
      it != inputs.end();
      ++it) {
    if ((*it) == oldInstr) {
      *it = newInstr;
    }
  }

  // this happens during delete
  if (newInstr != NULL) {
    newInstr->usages.insert(this);
  }
}

void Instruction::replaceUsagesWith(Instruction* newInstr) {
  for (set<Instruction*>::iterator it = usages.begin();
      it != usages.end();
      ++it) {
    (*it)->replaceInput(this, newInstr);
  }

  usages.clear();
}

void Instruction::append(Instruction* toAppend) {
  toAppend->block = block;
  Instruction* toFollow = next;
  toAppend->next = toFollow;
  next = toAppend;
}


Instruction::~Instruction() {
  // have to be careful freeing, because there are a lot of
  // weak pointers that matter here. Take
  // ret foo; var x = y; if x do ret x; end
  // we delete 'var x' before 'ret x', since the `next` chain
  // doesn't cross block boundaries.
  // However, those are dead code, se we can replace the dangling
  // pointers with NULL and the freeing chain should work out.
  replaceUsagesWith(NULL);

  for (vector<Instruction*>::iterator it = inputs.begin();
      it != inputs.end();
      ++it) {
    if (*it != NULL) {
      (*it)->usages.erase(this);
    }
  }

  if (next != NULL) {
    delete next;
  }
}
