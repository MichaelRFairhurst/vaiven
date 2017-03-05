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

namespace vaiven { namespace visitor {

using std::map;
using std::stack;
using std::string;
using std::vector;

template<typename T>
class stack_with_container : public stack<T> {
  public:
    using std::stack<T>::c; // expose the container
};

using namespace vaiven::ast;

class Interpreter : public Visitor<> {

  public:
  Interpreter(Functions& funcs) : funcs(funcs) {};
  //int interpret(Node<>& root, vector<Value> args, map<string, int>* variablesMap);
  Value interpret(Node<>& root);

  virtual void visitAssignmentExpression(AssignmentExpression<>& expr);
  virtual void visitAdditionExpression(AdditionExpression<>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<>& expr);
  virtual void visitDivisionExpression(DivisionExpression<>& expr);
  virtual void visitIntegerExpression(IntegerExpression<>& expr);
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
  virtual void visitExpressionStatement(ExpressionStatement<>& stmt);
  virtual void visitIfStatement(IfStatement<>& stmt);
  virtual void visitReturnStatement(ReturnStatement<>& stmt);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);
  virtual void visitVarDecl(VarDecl<>& varDecl);

  stack_with_container<Value> stack;
  map<string, int>* variablesMap;
  Functions& funcs;
};

}}

#endif
