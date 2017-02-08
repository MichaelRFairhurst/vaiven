#ifndef VAIVEN_VISITOR_HEADER_INTERPRETER
#define VAIVEN_VISITOR_HEADER_INTERPRETER

#include <map>
#include <stack>
#include <string>
#include <vector>

#include "../ast/visitor.h"
#include "../ast/all.h"

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
  //int interpret(Node<>& root, vector<int> args, map<string, int>* variablesMap);
  int interpret(Node<>& root);

  virtual void visitAdditionExpression(AdditionExpression<>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<>& expr);
  virtual void visitDivisionExpression(DivisionExpression<>& expr);
  virtual void visitIntegerExpression(IntegerExpression<>& expr);
  virtual void visitVariableExpression(VariableExpression<>& expr);
  virtual void visitExpressionStatement(ExpressionStatement<>& expr);
  virtual void visitBlock(Block<>& expr);
  virtual void visitFuncDecl(FuncDecl<>& funcDecl);

  stack_with_container<int> stack;
  map<string, int>* variablesMap;
};

}}

#endif
