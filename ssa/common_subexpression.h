#ifndef VAIVEN_VISITOR_HEADER_SSA_COMMON_SUBEXPRESSION
#define VAIVEN_VISITOR_HEADER_SSA_COMMON_SUBEXPRESSION

#include <unordered_map>

#include "forward_visitor.h"
#include "../value.h"

namespace std {
  
  template<>
  struct hash<vaiven::ssa::InstructionType> {
    size_t operator()(vaiven::ssa::InstructionType const& type) const {
      return hash<int>{}((int) type);
    }
  };
  
  template<>
  struct hash<vaiven::Value> {
    size_t operator()(vaiven::Value const& val) const {
      return hash<uint64_t>{}(val.getRaw());
    }
  };
  
  template<>
  struct equal_to<vaiven::Value> {
    bool operator()(vaiven::Value const& valA, vaiven::Value const& valB) const {
      return valA.getRaw() == valB.getRaw();
    }
  };
  
  template<typename A, typename B>
  struct hash<pair<A, B> > {
    size_t operator()(pair<A, B> const& vals) const {
      return hash<decltype(vals.first)>{}(vals.first) * 31 + hash<decltype(vals.second)>{}(vals.second);
    }
  };
  
  template<typename A>
  struct hash<vector<A> > {
    size_t operator()(vector<A> const& vec) const {
      size_t acc = hash<int>{}(vec.size());

      for (auto it = vec.begin(); it != vec.end(); ++it) {
        acc *= 31;
        acc += hash<A>{}(*it);
      }

      return acc;
    }
  };
}

namespace vaiven { namespace ssa {

class CommonSubexpressionEliminator : public ForwardVisitor {
  public:
  CommonSubexpressionEliminator() : performedWork(false) {};

  void visitPhiInstr(PhiInstr& instr);
  void visitArgInstr(ArgInstr& instr);
  void visitConstantInstr(ConstantInstr& instr);
  void visitCallInstr(CallInstr& instr);
  void visitTypecheckInstr(TypecheckInstr& instr);
  void visitBoxInstr(BoxInstr& instr);
  void visitAddInstr(AddInstr& instr);
  void visitIntAddInstr(IntAddInstr& instr);
  void visitStrAddInstr(StrAddInstr& instr);
  void visitSubInstr(SubInstr& instr);
  void visitMulInstr(MulInstr& instr);
  void visitDivInstr(DivInstr& instr);
  void visitNotInstr(NotInstr& instr);
  void visitCmpEqInstr(CmpEqInstr& instr);
  void visitCmpIneqInstr(CmpIneqInstr& instr);
  void visitCmpGtInstr(CmpGtInstr& instr);
  void visitCmpGteInstr(CmpGteInstr& instr);
  void visitCmpLtInstr(CmpLtInstr& instr);
  void visitCmpLteInstr(CmpLteInstr& instr);
  void visitDynamicAccessInstr(DynamicAccessInstr& instr);
  void visitDynamicStoreInstr(DynamicStoreInstr& instr);
  void visitListAccessInstr(ListAccessInstr& instr);
  void visitListStoreInstr(ListStoreInstr& instr);
  void visitListInitInstr(ListInitInstr& instr);
  void visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr);
  void visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr);
  void visitObjectAccessInstr(ObjectAccessInstr& instr);
  void visitObjectStoreInstr(ObjectStoreInstr& instr);
  void visitErrInstr(ErrInstr& instr);
  void visitRetInstr(RetInstr& instr);
  void visitJmpCcInstr(JmpCcInstr& instr);

  bool visitFoldableInstruction(Instruction& instr);

  bool performedWork;

  std::unordered_map<std::pair<InstructionType, std::vector<Instruction*> >, std::vector<Instruction*> >
    prevInstructions;
  std::unordered_map<Value, std::vector<Instruction*> > constants;
   
};

}}

#endif
