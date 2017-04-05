#ifndef VAIVEN_VISITOR_HEADER_INTERPRETER
#define VAIVEN_VISITOR_HEADER_INTERPRETER

#include <map>
#include <stack>
#include <string>
#include <vector>

#include "../ast/visitor.h"
#include "../ast/all.h"
#include "../functions.h"
#include "../value.h"
#include "../scope.h"
#include "../util.h"
#include "../heap.h"

namespace vaiven { namespace visitor {

using std::map;
using std::stack;
using std::string;
using std::vector;

using namespace vaiven::ast;

class Interpreter : public Visitor<> {

  public:
  Interpreter(Functions& funcs) : funcs(funcs), heap(stack, scope) {
    globalHeap = &heap;
  };
  ~Interpreter() {
    globalHeap = NULL;
  }
  //int interpret(Node<>& root, vector<Value> args, map<string, int>* variablesMap);
  Value interpret(Node<>& root);

  virtual void visitAssignmentExpression(AssignmentExpression<>& expr);
  virtual void visitAdditionExpression(AdditionExpression<>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<>& expr);
  virtual void visitDivisionExpression(DivisionExpression<>& expr);
  virtual void visitIntegerExpression(IntegerExpression<>& expr);
  virtual void visitDoubleExpression(DoubleExpression<>& expr);
  virtual void visitStringExpression(StringExpression<>& expr);
  virtual void visitVariableExpression(VariableExpression<>& expr);
  virtual void visitBoolLiteral(BoolLiteral<>& expr);
  virtual void visitNotExpression(NotExpression<>& expr);
  virtual void visitEqualityExpression(EqualityExpression<>& expr);
  virtual void visitInequalityExpression(InequalityExpression<>& expr);
  virtual void visitGtExpression(GtExpression<>& expr);
  virtual void visitGteExpression(GteExpression<>& expr);
  virtual void visitLtExpression(LtExpression<>& expr);
  virtual void visitLteExpression(LteExpression<>& expr);
  virtual void visitFuncCallExpression(FuncCallExpression<>& expr);
  virtual void visitListLiteralExpression(ListLiteralExpression<>& expr);
  virtual void visitDynamicAccessExpression(DynamicAccessExpression<>& expr);
  virtual void visitDynamicStoreExpression(DynamicStoreExpression<>& expr);
  virtual void visitStaticAccessExpression(StaticAccessExpression<>& expr);
  virtual void visitStaticStoreExpression(StaticStoreExpression<>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<>& stmt);
  virtual void visitIfStatement(IfStatement<>& stmt);
  virtual void visitForCondition(ForCondition<>& stmt);
  virtual void visitReturnStatement(ReturnStatement<>& stmt);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);
  virtual void visitVarDecl(VarDecl<>& varDecl);

  stack_with_container<Value> stack;
  Functions& funcs;
  Scope<Value> scope;
  Heap heap;
};

}}

#endif
