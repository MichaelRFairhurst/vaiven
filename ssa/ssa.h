#ifndef VAIVEN_VISITOR_HEADER_SSA
#define VAIVEN_VISITOR_HEADER_SSA

#include "../asmjit/src/asmjit/asmjit.h"
#include "../type_info.h"
#include "../value.h"
#include "../runtime_error.h"

#include <string>
#include <vector>
#include <set>

using std::string;
using std::vector;
using std::set;

namespace vaiven { namespace ssa {

enum InstructionType {
  INSTR_PHI,
  INSTR_ARG,
  INSTR_CONSTANT,
  INSTR_CALL,
  INSTR_TYPECHECK,
  INSTR_BOX,
  INSTR_ADD,
  INSTR_SUB,
  INSTR_MUL,
  INSTR_DIV,
  INSTR_NOT,
  INSTR_CMPEQ,
  INSTR_CMPINEQ,
  INSTR_CMPGT,
  INSTR_CMPGTE,
  INSTR_CMPLT,
  INSTR_CMPLTE,
  INSTR_ERR,
  INSTR_RET,
  INSTR_JMPCC,
};

class SsaVisitor;

class Instruction {

  public:
  Instruction(InstructionType tag, VaivenStaticType type, bool isBoxed)
    : tag(tag), type(type), isBoxed(isBoxed), next(NULL) {};

  virtual void accept(SsaVisitor& visitor)=0;

  void replaceInput(Instruction* oldInstr, Instruction* newInstr);
  void replaceUsagesWith(Instruction* newInstr);
  void append(Instruction* toAppend);
  virtual ~Instruction();

  InstructionType tag;
  VaivenStaticType type;
  bool isBoxed;
  vector<Instruction*> inputs;
  set<Instruction*> usages;
  Instruction* next;
  asmjit::X86Gp out;
};

class ArgInstr;
class PhiInstr;
class ConstantInstr;
class CallInstr;
class TypecheckInstr;
class BoxInstr;
class AddInstr;
class SubInstr;
class MulInstr;
class DivInstr;
class NotInstr;
class CmpEqInstr;
class CmpIneqInstr;
class CmpGtInstr;
class CmpGteInstr;
class CmpLtInstr;
class CmpLteInstr;
class ErrInstr;
class RetInstr;
class JmpCcInstr;

// from cfg.h
class Block;
class UnconditionalBlockExit;
class ConditionalBlockExit;

class SsaVisitor {
  public:
  virtual void visitPhiInstr(PhiInstr& instr)=0;
  virtual void visitArgInstr(ArgInstr& instr)=0;
  virtual void visitConstantInstr(ConstantInstr& instr)=0;
  virtual void visitCallInstr(CallInstr& instr)=0;
  virtual void visitTypecheckInstr(TypecheckInstr& instr)=0;
  virtual void visitBoxInstr(BoxInstr& instr)=0;
  virtual void visitAddInstr(AddInstr& instr)=0;
  virtual void visitSubInstr(SubInstr& instr)=0;
  virtual void visitMulInstr(MulInstr& instr)=0;
  virtual void visitDivInstr(DivInstr& instr)=0;
  virtual void visitNotInstr(NotInstr& instr)=0;
  virtual void visitCmpEqInstr(CmpEqInstr& instr)=0;
  virtual void visitCmpIneqInstr(CmpIneqInstr& instr)=0;
  virtual void visitCmpGtInstr(CmpGtInstr& instr)=0;
  virtual void visitCmpGteInstr(CmpGteInstr& instr)=0;
  virtual void visitCmpLtInstr(CmpLtInstr& instr)=0;
  virtual void visitCmpLteInstr(CmpLteInstr& instr)=0;
  virtual void visitErrInstr(ErrInstr& instr)=0;
  virtual void visitRetInstr(RetInstr& instr)=0;
  virtual void visitJmpCcInstr(JmpCcInstr& instr)=0;

  virtual void visitBlock(Block& block)=0;
  virtual void visitUnconditionalBlockExit(UnconditionalBlockExit& exit)=0;
  virtual void visitConditionalBlockExit(ConditionalBlockExit& exit)=0;
};

class PhiInstr : public Instruction {
  public:
  PhiInstr() : Instruction(INSTR_PHI, VAIVEN_STATIC_TYPE_UNKNOWN, true) {
  };
  PhiInstr(Instruction* optionA, Instruction* optionB)
      : Instruction(INSTR_PHI, VAIVEN_STATIC_TYPE_UNKNOWN, true) {
    inputs.push_back(optionA);
    inputs.push_back(optionB);
    optionA->usages.insert(this);
    optionB->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitPhiInstr(*this);
  }
};

class ArgInstr : public Instruction {
  public:
  ArgInstr(int argi, VaivenStaticType basedOnUsage)
      : argi(argi), Instruction(INSTR_ARG, basedOnUsage, true) {};

  int argi;

  void accept(SsaVisitor& visitor) {
    visitor.visitArgInstr(*this);
  }
};

class ConstantInstr : public Instruction {
  public:
  ConstantInstr(Value val) : val(val), Instruction(INSTR_CONSTANT, val.getStaticType(), false) {};

  Value val;

  void accept(SsaVisitor& visitor) {
    visitor.visitConstantInstr(*this);
  }
};

class CallInstr : public Instruction {
  public:
  CallInstr(string funcName)
      : funcName(funcName), Instruction(INSTR_CALL, VAIVEN_STATIC_TYPE_UNKNOWN, true) {};

  string funcName;

  void accept(SsaVisitor& visitor) {
    visitor.visitCallInstr(*this);
  }
};

class TypecheckInstr : public Instruction {
  public:
  TypecheckInstr(Instruction* what, VaivenStaticType checkType)
      : Instruction(INSTR_TYPECHECK, checkType, false) {
    inputs.push_back(what);
    what->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitTypecheckInstr(*this);
  }
};

class BoxInstr : public Instruction {
  public:
  BoxInstr(Instruction* what) : Instruction(INSTR_BOX, what->type, true) {
    inputs.push_back(what);
    what->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitBoxInstr(*this);
  }
};

class AddInstr : public Instruction {
  public:
  AddInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_ADD, VAIVEN_STATIC_TYPE_INT, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool hasConstRhs;
  int constRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitAddInstr(*this);
  }
};

class SubInstr : public Instruction {
  public:
  SubInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_SUB, VAIVEN_STATIC_TYPE_INT, false), hasConstLhs(false), isInverse(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool isInverse;
  bool hasConstLhs;
  int constLhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitSubInstr(*this);
  }
};

class MulInstr : public Instruction {
  public:
  MulInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_MUL, VAIVEN_STATIC_TYPE_INT, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool hasConstRhs;
  int constRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitMulInstr(*this);
  }
};

class DivInstr : public Instruction {
  public:
  DivInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DIV, VAIVEN_STATIC_TYPE_INT, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDivInstr(*this);
  }
};

class NotInstr : public Instruction {
  public:
  NotInstr(Instruction* expr) : Instruction(INSTR_NOT, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(expr);
    expr->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitNotInstr(*this);
  }
};

class CmpEqInstr : public Instruction {
  public:
  CmpEqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPEQ, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  CmpEqInstr(Instruction* lhs, int rhsI32)
      : Instruction(INSTR_CMPEQ, VAIVEN_STATIC_TYPE_BOOL, false),
      hasConstRhs(true), constI32Rhs(rhsI32) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constI32Rhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpEqInstr(*this);
  }
};

class CmpIneqInstr : public Instruction {
  public:
  CmpIneqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPINEQ, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  CmpIneqInstr(Instruction* lhs, int rhsI32)
      : Instruction(INSTR_CMPINEQ, VAIVEN_STATIC_TYPE_BOOL, false),
    hasConstRhs(true), constI32Rhs(rhsI32) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constI32Rhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpIneqInstr(*this);
  }
};

class CmpGtInstr : public Instruction {
  public:
  CmpGtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPGT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  CmpGtInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_CMPGT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpGtInstr(*this);
  }
};

class CmpGteInstr : public Instruction {
  public:
  CmpGteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPGTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  CmpGteInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_CMPGTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpGteInstr(*this);
  }
};

class CmpLtInstr : public Instruction {
  public:
  CmpLtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPLT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  CmpLtInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_CMPLT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpLtInstr(*this);
  }
};

class CmpLteInstr : public Instruction {
  public:
  CmpLteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPLTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  CmpLteInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_CMPLTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpLteInstr(*this);
  }
};

class ErrInstr : public Instruction {
  public:
  ErrInstr(vaiven::ErrorCode error) : Instruction(INSTR_ERR, VAIVEN_STATIC_TYPE_VOID, false), error(error) {};
  vaiven::ErrorCode error;

  void accept(SsaVisitor& visitor) {
    visitor.visitErrInstr(*this);
  }
};

class RetInstr : public Instruction {
  public:
  RetInstr(Instruction* val) : Instruction(INSTR_RET, VAIVEN_STATIC_TYPE_VOID, false) {
    inputs.push_back(val);
    val->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitRetInstr(*this);
  }
};

class JmpCcInstr : public Instruction {
  public:
  JmpCcInstr(Instruction* val) : Instruction(INSTR_JMPCC, VAIVEN_STATIC_TYPE_VOID, false) {
    inputs.push_back(val);
    val->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitJmpCcInstr(*this);
  }
};

}}

#endif
