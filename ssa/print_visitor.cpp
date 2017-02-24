#include "print_visitor.h"

#include <iostream>

using namespace vaiven::ssa;
using namespace std;

void PrintVisitor::visitPhiInstr(PhiInstr& instr) {
  cout << "phi" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitArgInstr(ArgInstr& instr) {
  cout << "arg" << instr.argi << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitConstantInstr(ConstantInstr& instr) {
  cout << "val" << instr.val.getRaw() << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitCallInstr(CallInstr& instr) {
  cout << "call " << instr.funcName << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitTypecheckInstr(TypecheckInstr& instr) {
  cout << "typecheck" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitBoxInstr(BoxInstr& instr) {
  cout << "box" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitAddInstr(AddInstr& instr) {
  cout << "add" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitSubInstr(SubInstr& instr) {
  cout << "sub" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitMulInstr(MulInstr& instr) {
  cout << "mul" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitDivInstr(DivInstr& instr) {
  cout << "div" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitNotInstr(NotInstr& instr) {
  cout << "not" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitCmpEqInstr(CmpEqInstr& instr) {
  cout << "==" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitCmpIneqInstr(CmpIneqInstr& instr) {
  cout << "!=" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitCmpGtInstr(CmpGtInstr& instr) {
  cout << "gt" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitCmpGteInstr(CmpGteInstr& instr) {
  cout << "gte" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitCmpLtInstr(CmpLtInstr& instr) {
  cout << "lt" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitCmpLteInstr(CmpLteInstr& instr) {
  cout << "lte" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitErrInstr(ErrInstr& instr) {
  cout << "err" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}

void PrintVisitor::visitRetInstr(RetInstr& instr) {
  cout << "ret" << endl;
  if (instr.next != NULL) instr.next->accept(*this);
}
