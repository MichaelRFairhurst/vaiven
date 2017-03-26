#include "instruction_combiner.h"

using namespace vaiven::ssa;
using namespace std;
using namespace asmjit;

#include "print_visitor.h"
#include "../heap.h"

#define MATCH_INPUTS(guards, action) \
{ \
  Instruction* underCheck = &instr; \
  int inputOffset = 0; \
  do { \
    guards \
    action \
    performedWork = true; \
    return; \
  } while(false); \
}

#define INSTR(type) \
  if (underCheck->inputs[inputOffset]->tag != INSTR_ ## type) { \
    break; \
  } \

#define THEN ++inputOffset;

#define OF(guards_and_captures) \
  Instruction* save = underCheck; \
  underCheck = underCheck->inputs[inputOffset]; \
  int offsetSave = inputOffset; \
  inputOffset = 0; \
  guards_and_captures \
  underCheck = save; \
  inputOffset = offsetSave; \

#define CAPTURE_TYPE(type, name) \
  type* name = static_cast<type*>(underCheck->inputs[inputOffset]);

#define WHERE(cond) \
  if (!(cond)) { \
    break; \
  }

#define SOME(name) \
  Instruction* name = underCheck->inputs[inputOffset];

#define CONSTANT(name)  \
  INSTR(CONSTANT) \
  CAPTURE_TYPE(ConstantInstr, name);

#define INT_CONSTANT(name) \
  INSTR(CONSTANT) \
  CAPTURE_TYPE(ConstantInstr, name ## _instr); \
  int name = name ## _instr->val.getInt();

#define BOOL_CONSTANT(name) \
  INSTR(CONSTANT) \
  CAPTURE_TYPE(ConstantInstr, name ## _instr); \
  bool name = name ## _instr->val.getBool();

#define STRING_CONSTANT(name) \
  INSTR(CONSTANT) \
  CAPTURE_TYPE(ConstantInstr, name ## _instr); \
  string name = ((GcableString*) name ## _instr->val.getPtr())->str;

void InstructionCombiner::visitPhiInstr(PhiInstr& instr) {
  // breaking apart nested phis ie phi(x, phi(y, z)) into phi(x, y, z) improves DCE
  vector<Instruction*> innerPhis;

  vector<Instruction*>::iterator it = instr.inputs.begin();
  while (it != instr.inputs.end()) {
    if (*it == NULL) {
      it = instr.inputs.erase(it);
      continue;
    }
    if ((*it)->tag == INSTR_PHI) {
      innerPhis.push_back(*it);
      (*it)->usages.erase(&instr);
      it = instr.inputs.erase(it);
      performedWork = true;
    } else {
      ++it;
    }
  }

  for (vector<Instruction*>::iterator it = innerPhis.begin(); it != innerPhis.end(); ++it) {
    instr.inputs.insert(instr.inputs.end(), (*it)->inputs.begin(), (*it)->inputs.end());
  }

  for (vector<Instruction*>::iterator it = instr.inputs.begin(); it != instr.inputs.end(); ++it) {
    (*it)->usages.insert(&instr);
  }
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
}

void InstructionCombiner::visitAddInstr(AddInstr& instr) {
  // TODO anything we can do here?
}

void InstructionCombiner::visitStrAddInstr(StrAddInstr& instr) {
  // x + "foo" + "bar" == x + "foobar"
  MATCH_INPUTS(
    INSTR(STR_ADD) OF(SOME(lift) THEN STRING_CONSTANT(a))
    THEN STRING_CONSTANT(b),
    // TODO don't leak this gcable string
    replaceReferencingNewConstant(instr, new StrAddInstr(lift, new ConstantInstr(new GcableString(a + b))));
  )
}

void InstructionCombiner::visitIntAddInstr(IntAddInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  // x + 2 + 4 == x + 6
  MATCH_INPUTS(
    INSTR(INT_ADD) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new IntAddInstr(lift, new ConstantInstr(a + b)));
  )

  // x + 0 == x
  MATCH_INPUTS(
    SOME(identity) THEN INT_CONSTANT(a) WHERE(a == 0),
    instr.replaceUsagesWith(identity);
  )

  // x + x == x * 2
  MATCH_INPUTS(
    SOME(lhs) THEN SOME(rhs) WHERE(lhs == rhs),
    replaceReferencingNewConstant(instr, new MulInstr(lhs, new ConstantInstr(2)));
  )

  // (z - x) + x == z
  MATCH_INPUTS(
    INSTR(SUB) OF (SOME(identity) THEN SOME(a))
    THEN SOME(b) WHERE(a == b),
    instr.replaceUsagesWith(identity);
  )

  // (x * 2) + x == x * 3
  MATCH_INPUTS(
    INSTR(MUL) OF (SOME(lift) THEN INT_CONSTANT(factor))
    THEN SOME(additive) WHERE(lift == additive),
    replaceReferencingNewConstant(instr, new MulInstr(lift, new ConstantInstr(factor + 1)));
  )

  // (2 - x) + 1 == (3 - x)
  MATCH_INPUTS(
    INSTR(SUB) OF (INT_CONSTANT(a) THEN SOME(lift))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new SubInstr(new ConstantInstr(a + b), lift));
  )
}

void InstructionCombiner::visitSubInstr(SubInstr& instr) {
  // turn x - 4 into x + -4 so that visitIntAddInstr can do most of the heavy lifting
  if (instr.inputs[1]->tag == INSTR_CONSTANT) {
    ConstantInstr* myConstant = static_cast<ConstantInstr*>(instr.inputs[1]);
    int newval = -myConstant->val.getInt();

    ConstantInstr* newConstant = new ConstantInstr(newval);
    IntAddInstr* addInstr = new IntAddInstr(instr.inputs[0], newConstant);
    instr.append(newConstant);
    newConstant->append(addInstr);
    instr.replaceUsagesWith(addInstr);
  }

  // (x * 3) - x == x * 2
  MATCH_INPUTS(
    INSTR(MUL) OF (SOME(lift) THEN INT_CONSTANT(factor))
    THEN SOME(additive) WHERE(lift == additive),
    replaceReferencingNewConstant(instr, new MulInstr(lift, new ConstantInstr(factor - 1)));
  )

  // x - x == 0
  MATCH_INPUTS(
    SOME(identity) THEN INT_CONSTANT(a) WHERE(a == 0),
    ConstantInstr* constantInstr = new ConstantInstr(Value(0));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
  )

  // (z + x) - x == z
  MATCH_INPUTS(
    INSTR(INT_ADD) OF(SOME(identity) THEN SOME(a))
    THEN SOME(b) WHERE(a == b),
    instr.replaceUsagesWith(identity);
  );

  // (x + z) - x == z
  MATCH_INPUTS(
    INSTR(INT_ADD) OF(SOME(a) THEN SOME(identity))
    THEN SOME(b) WHERE(a == b),
    instr.replaceUsagesWith(identity);
  );

  // x - (z + x) == z
  MATCH_INPUTS(
    SOME(a) THEN
    INSTR(INT_ADD) OF(SOME(identity) THEN SOME(b))
    WHERE(a == b),
    instr.replaceUsagesWith(identity);
  );

  // x - (x + z) == z
  MATCH_INPUTS(
    SOME(a) THEN
    INSTR(INT_ADD) OF(SOME(b) THEN SOME(identity))
    WHERE(a == b),
    instr.replaceUsagesWith(identity);
  );

  // 4 - (2 - x) == 4 + -(2 - x) == 4 + (-2 + x) == 2 + x == x + 2
  // don't have to handle (x - 2) - 2 because those become (x + -2) + -2
  MATCH_INPUTS(
    INT_CONSTANT(a) THEN
    INSTR(SUB) OF(INT_CONSTANT(b) THEN SOME(lift)),
    replaceReferencingNewConstant(instr, new IntAddInstr(lift, new ConstantInstr(a - b)));
  );

  // 4 - (x + 2) == 4 -(2 + x) == 4 + (-2 - x) == 2 - x == x + -2
  MATCH_INPUTS(
    INT_CONSTANT(a) THEN
    INSTR(INT_ADD) OF(SOME(lift) THEN INT_CONSTANT(b)),
    replaceReferencingNewConstant(instr, new IntAddInstr(lift, new ConstantInstr(a - b)));
  );
}

void InstructionCombiner::visitMulInstr(MulInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  // x * 1 == x
  MATCH_INPUTS(
    SOME(identity) THEN INT_CONSTANT(factor) WHERE(factor == 1),
    instr.replaceUsagesWith(identity);
  )

  // x * 0 == 0
  MATCH_INPUTS(
    SOME(_) THEN INT_CONSTANT(zero) WHERE(zero == 0),
    instr.replaceUsagesWith(zero_instr);
  )

   // (x * 2) * 3 -> (x * 6)
  MATCH_INPUTS(
    INSTR(MUL) OF (SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new MulInstr(lift, new ConstantInstr(a * b)));
  )

  // can't naively optimize x / 4 * 4 because of integer math & rounding
}

void InstructionCombiner::visitDivInstr(DivInstr& instr) {

  // x / x == 1
  MATCH_INPUTS(
    SOME(a) THEN SOME(b) WHERE(a == b),
    ConstantInstr* constantInstr = new ConstantInstr(Value(1));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
  )

  // x / 1 == x
  MATCH_INPUTS(
    SOME(identity) THEN INT_CONSTANT(one) WHERE(one == 1),
    instr.replaceUsagesWith(identity);
  );

  // (z * x) / x == z
  MATCH_INPUTS(
    INSTR(MUL) OF(SOME(identity) THEN SOME(a))
    THEN SOME(b) WHERE(a == b),
    instr.replaceUsagesWith(identity);
  );

  // (x * z) / x == z
  MATCH_INPUTS(
    INSTR(MUL) OF(SOME(a) THEN SOME(identity))
    THEN SOME(b) WHERE(a == b),
    instr.replaceUsagesWith(identity);
  );

  // (x / 2) / 3 == x / 6
  MATCH_INPUTS(
    INSTR(DIV) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new DivInstr(lift, new ConstantInstr(a * b)));
  );

  // (x * 2) / 4 == x / 2
  MATCH_INPUTS(
    INSTR(MUL) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) WHERE(b % a == 0),
    replaceReferencingNewConstant(instr, new DivInstr(lift, new ConstantInstr(a / b)));
  );

  // (4 / x) / 2 == 2 / x
  MATCH_INPUTS(
    INSTR(DIV) OF(INT_CONSTANT(a) THEN SOME(lift))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new DivInstr(lift, new ConstantInstr(b / a)));
  );

  // 4 / (x * 2) == 2 / x
  MATCH_INPUTS(
    INT_CONSTANT(a) THEN
    INSTR(MUL) OF(SOME(lift) THEN INT_CONSTANT(b)) WHERE(a % b == 0),
    replaceReferencingNewConstant(instr, new DivInstr(new ConstantInstr(a / b), lift));
  );

  // 4 / (x / 2) == 8 / x
  MATCH_INPUTS(
    INT_CONSTANT(a) THEN
    INSTR(DIV) OF(SOME(lift) THEN INT_CONSTANT(b)),
    replaceReferencingNewConstant(instr, new DivInstr(new ConstantInstr(a * b), lift));
  );
}

void InstructionCombiner::visitNotInstr(NotInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CMPGT) {
    CmpLteInstr* lte = new CmpLteInstr(instr.inputs[0]->inputs[0], instr.inputs[0]->inputs[1]);
    instr.append(lte);
    instr.replaceUsagesWith(lte);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_CMPGTE) {
    CmpLtInstr* lt = new CmpLtInstr(instr.inputs[0]->inputs[0], instr.inputs[0]->inputs[1]);
    instr.append(lt);
    instr.replaceUsagesWith(lt);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_CMPLT) {
    CmpGteInstr* gte = new CmpGteInstr(instr.inputs[0]->inputs[0], instr.inputs[0]->inputs[1]);
    instr.append(gte);
    instr.replaceUsagesWith(gte);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_CMPLTE) {
    CmpGtInstr* gt = new CmpGtInstr(instr.inputs[0]->inputs[0], instr.inputs[0]->inputs[1]);
    instr.append(gt);
    instr.replaceUsagesWith(gt);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_CMPEQ) {
    CmpIneqInstr* ineq = new CmpIneqInstr(instr.inputs[0]->inputs[0], instr.inputs[0]->inputs[1]);
    instr.append(ineq);
    instr.replaceUsagesWith(ineq);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_CMPINEQ) {
    CmpEqInstr* eq = new CmpEqInstr(instr.inputs[0]->inputs[0], instr.inputs[0]->inputs[1]);
    instr.append(eq);
    instr.replaceUsagesWith(eq);
    performedWork = true;
  } else if (instr.inputs[0]->tag == INSTR_NOT) {
    instr.replaceUsagesWith(instr.inputs[0]->inputs[0]);
    performedWork = true;
  }
}

void InstructionCombiner::visitCmpEqInstr(CmpEqInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  if (instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[1]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[0]->type != instr.inputs[1]->type) {
    ConstantInstr* constantInstr = new ConstantInstr(Value(false));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
    performedWork = true;
  }

  // (x + 1) == 3 can be x == 2
  MATCH_INPUTS(
    INSTR(INT_ADD) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new CmpEqInstr(lift, new ConstantInstr(b - a)));
  );

  // (3 - x) == 1 can be x == 2
  MATCH_INPUTS(
    INSTR(SUB) OF(INT_CONSTANT(a) THEN SOME(lift))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new CmpEqInstr(lift, new ConstantInstr(a - b)));
  );

  // x * 2 == 3 is just false in integer math
  MATCH_INPUTS(
    INSTR(MUL) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) WHERE(b % a != 0),
    ConstantInstr* constantInstr = new ConstantInstr(Value(false));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
  );

  // (x * 2) == 4 can be x == 2
  MATCH_INPUTS(
    INSTR(MUL) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) ,
    replaceReferencingNewConstant(instr, new CmpEqInstr(lift, new ConstantInstr(a - b)));
  );

  // (x / 2) == 4 can be x == 8
  MATCH_INPUTS(
    INSTR(DIV) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) ,
    replaceReferencingNewConstant(instr, new CmpEqInstr(lift, new ConstantInstr(a * b)));
  );

  // (4 / x) == 2 can be x == 2
  MATCH_INPUTS(
    INSTR(DIV) OF(INT_CONSTANT(a) THEN SOME(lift))
    THEN INT_CONSTANT(b) WHERE (a % b == 0),
    replaceReferencingNewConstant(instr, new CmpEqInstr(lift, new ConstantInstr(a / b)));
  );

  // (4 / x) == 3 is just false in integer math
  MATCH_INPUTS(
    INSTR(DIV) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) WHERE(b % a != 0),
    ConstantInstr* constantInstr = new ConstantInstr(Value(false));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
  );
}

void InstructionCombiner::visitCmpIneqInstr(CmpIneqInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    std::swap(instr.inputs[0], instr.inputs[1]);
  }

  if (instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[0]->type != VAIVEN_STATIC_TYPE_UNKNOWN
      && instr.inputs[0]->type != instr.inputs[1]->type) {
    ConstantInstr* constantInstr = new ConstantInstr(Value(true));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
    performedWork = true;
  }

  // (x + 1) != 3 can be x != 2
  MATCH_INPUTS(
    INSTR(INT_ADD) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new CmpIneqInstr(lift, new ConstantInstr(b - a)));
  );

  // (3 - x) != 1 can be x != 2
  MATCH_INPUTS(
    INSTR(SUB) OF(INT_CONSTANT(a) THEN SOME(lift))
    THEN INT_CONSTANT(b),
    replaceReferencingNewConstant(instr, new CmpIneqInstr(lift, new ConstantInstr(a - b)));
  );

  // x * 2 != 3 is just true in integer math
  MATCH_INPUTS(
    INSTR(MUL) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) WHERE(b % a != 0),
    ConstantInstr* constantInstr = new ConstantInstr(Value(true));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
  );

  // (x * 2) != 4 can be x != 2
  MATCH_INPUTS(
    INSTR(MUL) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) ,
    replaceReferencingNewConstant(instr, new CmpIneqInstr(lift, new ConstantInstr(a - b)));
  );

  // (x / 2) != 4 can be x != 8
  MATCH_INPUTS(
    INSTR(DIV) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) ,
    replaceReferencingNewConstant(instr, new CmpIneqInstr(lift, new ConstantInstr(a * b)));
  );

  // (4 / x) != 2 can be x != 2
  MATCH_INPUTS(
    INSTR(DIV) OF(INT_CONSTANT(a) THEN SOME(lift))
    THEN INT_CONSTANT(b) WHERE (a % b == 0),
    replaceReferencingNewConstant(instr, new CmpIneqInstr(lift, new ConstantInstr(a / b)));
  );

  // (4 / x) != 3 is just true in integer math
  MATCH_INPUTS(
    INSTR(DIV) OF(SOME(lift) THEN INT_CONSTANT(a))
    THEN INT_CONSTANT(b) WHERE(b % a != 0),
    ConstantInstr* constantInstr = new ConstantInstr(Value(true));
    instr.append(constantInstr);
    instr.replaceUsagesWith(constantInstr);
  );
}

void InstructionCombiner::visitCmpGtInstr(CmpGtInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    CmpLtInstr* inverted = new CmpLtInstr(instr.inputs[1], instr.inputs[0]);
    instr.append(inverted);
    instr.replaceUsagesWith(inverted);
    performedWork = true;
  }
}

void InstructionCombiner::visitCmpGteInstr(CmpGteInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    CmpLteInstr* inverted = new CmpLteInstr(instr.inputs[1], instr.inputs[0]);
    instr.append(inverted);
    instr.replaceUsagesWith(inverted);
    performedWork = true;
  }
}

void InstructionCombiner::visitCmpLtInstr(CmpLtInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    CmpGtInstr* inverted = new CmpGtInstr(instr.inputs[1], instr.inputs[0]);
    instr.append(inverted);
    instr.replaceUsagesWith(inverted);
    performedWork = true;
  }
}

void InstructionCombiner::visitCmpLteInstr(CmpLteInstr& instr) {
  if (instr.inputs[0]->tag == INSTR_CONSTANT) {
    CmpGteInstr* inverted = new CmpGteInstr(instr.inputs[1], instr.inputs[0]);
    instr.append(inverted);
    instr.replaceUsagesWith(inverted);
    performedWork = true;
  }
}

void InstructionCombiner::visitDynamicAccessInstr(DynamicAccessInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitDynamicStoreInstr(DynamicStoreInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitListAccessInstr(ListAccessInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitListStoreInstr(ListStoreInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitListInitInstr(ListInitInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitObjectAccessInstr(ObjectAccessInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitObjectStoreInstr(ObjectStoreInstr& instr) {
  // TODO anything to combine here?
}

void InstructionCombiner::visitErrInstr(ErrInstr& instr) {
}

void InstructionCombiner::visitRetInstr(RetInstr& instr) {
}

void InstructionCombiner::visitJmpCcInstr(JmpCcInstr& instr) {
}
