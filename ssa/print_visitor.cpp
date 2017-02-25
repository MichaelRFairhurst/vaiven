#include "print_visitor.h"

#include <iostream>

using namespace vaiven::ssa;
using namespace std;

void PrintVisitor::printInstruction(string prefix, Instruction* instr) {
  int id = varIds.size();
  varIds[instr] = id;

  cout << id << ": " << prefix;

  for (vector<Instruction*>::iterator it = instr->inputs.begin();
      it != instr->inputs.end();
      ++it) {
    if(it != instr->inputs.begin()) {
      cout << ",";
    }
    cout << " " << varIds[*it];
  }

  cout << endl;
}

void PrintVisitor::visitPhiInstr(PhiInstr& instr) {
  printInstruction("phi", &instr);
}

void PrintVisitor::visitArgInstr(ArgInstr& instr) {
  int id = varIds.size();
  varIds[&instr] = id;

  cout << id << ": arg" << instr.argi << endl;
}

void PrintVisitor::visitConstantInstr(ConstantInstr& instr) {
  int id = varIds.size();
  varIds[&instr] = id;

  cout << id << ": val" << instr.val.getRaw() << endl;
}

void PrintVisitor::visitCallInstr(CallInstr& instr) {
  printInstruction(instr.funcName, &instr);
}

void PrintVisitor::visitTypecheckInstr(TypecheckInstr& instr) {
  printInstruction("typecheck", &instr);
}

void PrintVisitor::visitBoxInstr(BoxInstr& instr) {
  printInstruction("box", &instr);
}

void PrintVisitor::visitAddInstr(AddInstr& instr) {
  printInstruction("add", &instr);
}

void PrintVisitor::visitSubInstr(SubInstr& instr) {
  printInstruction("sub", &instr);
}

void PrintVisitor::visitMulInstr(MulInstr& instr) {
  printInstruction("mul", &instr);
}

void PrintVisitor::visitDivInstr(DivInstr& instr) {
  printInstruction("div", &instr);
}

void PrintVisitor::visitNotInstr(NotInstr& instr) {
  printInstruction("not", &instr);
}

void PrintVisitor::visitCmpEqInstr(CmpEqInstr& instr) {
  printInstruction("==", &instr);
}

void PrintVisitor::visitCmpIneqInstr(CmpIneqInstr& instr) {
  printInstruction("!=", &instr);
}

void PrintVisitor::visitCmpGtInstr(CmpGtInstr& instr) {
  printInstruction("gt", &instr);
}

void PrintVisitor::visitCmpGteInstr(CmpGteInstr& instr) {
  printInstruction("gte", &instr);
}

void PrintVisitor::visitCmpLtInstr(CmpLtInstr& instr) {
  printInstruction("lt", &instr);
}

void PrintVisitor::visitCmpLteInstr(CmpLteInstr& instr) {
  printInstruction("lte", &instr);
}

void PrintVisitor::visitErrInstr(ErrInstr& instr) {
  printInstruction("err", &instr);
}

void PrintVisitor::visitRetInstr(RetInstr& instr) {
  printInstruction("ret", &instr);
}
