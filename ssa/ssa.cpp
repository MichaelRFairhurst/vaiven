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
  newInstr->usages.insert(this);
}

void Instruction::replaceUsagesWith(Instruction* newInstr) {
  for (set<Instruction*>::iterator it = usages.begin();
      it != usages.end();
      ++it) {
    (*it)->replaceInput(this, newInstr);
  }

  newInstr->usages.insert(usages.begin(), usages.end());
  usages.clear();
}

void Instruction::append(Instruction* toAppend) {
  Instruction* toFollow = next;
  toAppend->next = toFollow;
  next = toAppend;
}


Instruction::~Instruction() {
  for (vector<Instruction*>::iterator it = inputs.begin();
      it != inputs.end();
      ++it) {
    (*it)->usages.erase(this);
  }
}
