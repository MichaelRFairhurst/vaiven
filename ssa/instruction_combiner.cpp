#include "instruction_combiner.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

void InstructionCombiner::visitPhiInstr(PhiInstr& instr) {
}

void InstructionCombiner::visitArgInstr(ArgInstr& instr) {
}

void InstructionCombiner::visitConstantInstr(ConstantInstr& instr) {
}

void InstructionCombiner::visitCallInstr(CallInstr& instr) {
}

void InstructionCombiner::visitTypecheckInstr(TypecheckInstr& instr) {
}

void InstructionCombiner::visitBoxInstr(BoxInstr& instr) {
}

bool InstructionCombiner::isBinIntInstruction(Instruction& instr) {
  return instr.inputs[0]->type == VAIVEN_STATIC_TYPE_INT
      && instr.inputs[1]->type == VAIVEN_STATIC_TYPE_INT;
}

void InstructionCombiner::replaceReferencingNewConstant(Instruction& instr, Instruction* newInstr) {
  if (instr.usages.size() == 0) {
    return;
  }

  // sub generates constants on the left, add and mul on the right, div both.
  Instruction* constantInstr = newInstr->inputs[0]->tag == INSTR_CONSTANT
      ? newInstr->inputs[0] : newInstr->inputs[1];

  // add the constant that will replace this to the instruction list. Let dead code
  // elim remove this node later.
  instr.append(constantInstr);
  constantInstr->append(newInstr); // the instr using that constant

  instr.replaceUsagesWith(newInstr);
  performedWork = true;
}

void InstructionCombiner::visitAddInstr(AddInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  //if (isBinIntInstruction(instr)) {
  //  return;
  //}

  // TODO turn x + x + x into x * 3
  if (instr.inputs[1]->tag == INSTR_CONSTANT &&
      static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt() == 0) {
    instr.replaceUsagesWith(instr.inputs[0]);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_SUB
      && instr.inputs[0]->inputs[1] == instr.inputs[1]) {
    // (z - x) + x  == z
    instr.replaceUsagesWith(instr.inputs[0]->inputs[0]);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_SUB
      && instr.inputs[0]->inputs[0] == instr.inputs[1]) {
    // (x - z) + x  == z
    instr.replaceUsagesWith(instr.inputs[0]->inputs[1]);
    performedWork = true;
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_ADD
      && instr.inputs[0]->inputs[1]->tag == INSTR_CONSTANT) {
    // (x + 2) + 1 -> (x + 3)
    // this also handles rhs constant subtraction since x - 2 turns into x + -2
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[1]);
    int newval = myConstant->val.getInt() + theirConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    replaceReferencingNewConstant(instr,
        new AddInstr(instr.inputs[0]->inputs[0], newConstant));
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_SUB
      && instr.inputs[0]->inputs[0]->tag == INSTR_CONSTANT) {
    // (2 - x) + 1 -> (3 - x)
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[0]);
    int newval = myConstant->val.getInt() + theirConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    replaceReferencingNewConstant(instr,
        new SubInstr(newConstant, instr.inputs[0]->inputs[1]));
  }
}

void InstructionCombiner::visitSubInstr(SubInstr& instr) {
  // turn x - 4 into x + -4 so that visitAddInstr can do most of the heavy lifting
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    int newval = -myConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    AddInstr* addInstr = new AddInstr(instr.inputs[0], newConstant);
    instr.append(newConstant);
    newConstant->append(addInstr);
    instr.replaceUsagesWith(addInstr);
    performedWork = true;
  }

  //if (!isBinIntInstruction(instr)) {
  //  return;
  //}

  // TODO turn x * 2 - x into x
  if (instr.inputs[0] == instr.inputs[1]) {
    // x - x == 0
    ConstantInstr* constantInstr = new ConstantInstr(Value(0));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_ADD
      && instr.inputs[0]->inputs[1] == instr.inputs[1]) {
    // (z + x) - x  == z
    instr.replaceUsagesWith(instr.inputs[0]->inputs[0]);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_ADD
      && instr.inputs[0]->inputs[0] == instr.inputs[1]) {
    // (x + z) - x  == z
    instr.replaceUsagesWith(instr.inputs[0]->inputs[1]);
    performedWork = true;
  } else if (instr.inputs[1]->tag == INSTR_ADD
      && instr.inputs[1]->inputs[1] == instr.inputs[0]) {
    // x - (z + x)  == z
    instr.replaceUsagesWith(instr.inputs[1]->inputs[0]);
    performedWork = true;
  } else if (instr.inputs[1]->tag == INSTR_ADD
      && instr.inputs[1]->inputs[0] == instr.inputs[0]) {
    // x - (x + z)  == z
    instr.replaceUsagesWith(instr.inputs[1]->inputs[1]);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_CONSTANT
      && instr.inputs[1]->tag == INSTR_SUB
      && instr.inputs[1]->inputs[0]->tag == INSTR_CONSTANT) {
    // 4 - (2 - x) == 4 + -(2 - x) == 4 + (-2 + x) == 2 + x == x + 2
    // don't have to handle (x - 2) - 2 because those become (x + -2) + -2
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[0]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[1]->inputs[0]);
    int newval = myConstant->val.getInt() - theirConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    replaceReferencingNewConstant(instr,
        new AddInstr(instr.inputs[1]->inputs[1], newConstant));
  } else if (instr.inputs[0]->tag == INSTR_CONSTANT
      && instr.inputs[1]->tag == INSTR_ADD
      && instr.inputs[1]->inputs[1]->tag == INSTR_CONSTANT) {
    // 4 - (x + 2) == 4 -(2 + x) == 4 + (-2 - x) == 2 - x == x + -2
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[0]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[1]->inputs[1]);
    int newval = -(myConstant->val.getInt() + theirConstant->val.getInt());

    ConstantInstr* newConstant = new ConstantInstr(newval);
    replaceReferencingNewConstant(instr,
        new AddInstr(instr.inputs[1]->inputs[0], newConstant));
  }
}

void InstructionCombiner::visitMulInstr(MulInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  //if (!isBinIntInstruction(instr)) {
  //  return;
  //}

  if (instr.inputs[1]->tag == INSTR_CONSTANT &&
      static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt() == 1) {
    // x * 1 == x
    instr.replaceUsagesWith(instr.inputs[0]);
    performedWork = true;
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT &&
      static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt() == 0) {
    // x * 0 == 0
    instr.replaceUsagesWith(instr.inputs[1]);
    performedWork = true;
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_MUL
      && instr.inputs[0]->inputs[1]->tag == INSTR_CONSTANT) {
    // (x * 2) * 3 -> (x * 6)
    // this also handles rhs constant subtraction since x - 2 turns into x + -2
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[1]);
    int newval = myConstant->val.getInt() * theirConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    replaceReferencingNewConstant(instr,
        new MulInstr(instr.inputs[0]->inputs[0], newConstant));
  }
  // can't naively optimize x / 4 * 4 because of integer math & rounding
}

void InstructionCombiner::visitDivInstr(DivInstr& instr) {
  //if (!isBinIntInstruction(instr)) {
  //  return;
  //}

  if (instr.inputs[0] == instr.inputs[1]) {
    // x / x == 1
    ConstantInstr* constantInstr = new ConstantInstr(Value(1));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
    performedWork = true;
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT &&
      static_cast<ConstantInstr*>(instr.inputs[1])->val.getInt() == 1) {
    // x / 1 == x
    instr.replaceUsagesWith(instr.inputs[0]);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_MUL
      && instr.inputs[0]->inputs[1] == instr.inputs[1]) {
    // (z * x) / x  == z
    instr.replaceUsagesWith(instr.inputs[0]->inputs[0]);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_MUL
      && instr.inputs[0]->inputs[0] == instr.inputs[1]) {
    // (x * z) / x  == z
    instr.replaceUsagesWith(instr.inputs[0]->inputs[1]);
    performedWork = true;
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_DIV
      && instr.inputs[0]->inputs[1]->tag == INSTR_CONSTANT) {
    // (x / 2) / 3  == x / 6
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[1]);
    int newval = myConstant->val.getInt() * theirConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    replaceReferencingNewConstant(instr,
        new DivInstr(instr.inputs[0]->inputs[0], newConstant));
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_MUL
      && instr.inputs[0]->inputs[1]->tag == INSTR_CONSTANT) {
    // (x * 2) / 4  == x / 2
    // also handles (x * 2) / 4 since mul moves const to rhs
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[1]);
    // optimizable integer math only
    if (myConstant->val.getInt() % theirConstant->val.getInt() == 0) {
      int newval = myConstant->val.getInt() / theirConstant->val.getInt();

      ConstantInstr* newConstant = new ConstantInstr(newval);
      replaceReferencingNewConstant(instr,
          new DivInstr(instr.inputs[0]->inputs[0], newConstant));
    }
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_DIV
      && instr.inputs[0]->inputs[0]->tag == INSTR_CONSTANT) {
    // (4 / x) / 2  == 2 / x
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[0]);
    // optimizable integer math only
    if (theirConstant->val.getInt() % myConstant->val.getInt() == 0) {
      int newval = theirConstant->val.getInt() / myConstant->val.getInt();

      ConstantInstr* newConstant = new ConstantInstr(newval);
      replaceReferencingNewConstant(instr,
          new DivInstr(newConstant, instr.inputs[0]->inputs[1]));
    }
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_MUL
      && instr.inputs[0]->inputs[1]->tag == INSTR_CONSTANT) {
    // 4 / (x * 2) == 2 / x
    // also handles (2 * x) / 4 since mul moves const to rhs
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[1]);
    // optimizable integer math only
    if (myConstant->val.getInt() % theirConstant->val.getInt() == 0) {
      int newval = myConstant->val.getInt() / theirConstant->val.getInt();

      ConstantInstr* newConstant = new ConstantInstr(newval);
      replaceReferencingNewConstant(instr,
          new DivInstr(newConstant, instr.inputs[0]->inputs[0]));
    }
  } else if (instr.inputs[1]->tag == INSTR_CONSTANT
      && instr.inputs[0]->tag == INSTR_MUL
      && instr.inputs[0]->inputs[1]->tag == INSTR_CONSTANT) {
    // 4 / (x / 2) == 8 / x
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    ConstantInstr* theirConstant = static_cast<ConstantInstr*>(instr.inputs[0]->inputs[1]);
    int newval = myConstant->val.getInt() * theirConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    replaceReferencingNewConstant(instr,
        new DivInstr(newConstant, instr.inputs[0]->inputs[0]));
  }
}

void InstructionCombiner::visitNotInstr(NotInstr& instr) {
}

void InstructionCombiner::visitCmpEqInstr(CmpEqInstr& instr) {
}

void InstructionCombiner::visitCmpIneqInstr(CmpIneqInstr& instr) {
}

void InstructionCombiner::visitCmpGtInstr(CmpGtInstr& instr) {
}

void InstructionCombiner::visitCmpGteInstr(CmpGteInstr& instr) {
}

void InstructionCombiner::visitCmpLtInstr(CmpLtInstr& instr) {
}

void InstructionCombiner::visitCmpLteInstr(CmpLteInstr& instr) {
}

void InstructionCombiner::visitErrInstr(ErrInstr& instr) {
}

void InstructionCombiner::visitRetInstr(RetInstr& instr) {
}

void InstructionCombiner::visitJmpCcInstr(JmpCcInstr& instr) {
}
