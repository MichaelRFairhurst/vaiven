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

class Interpreter : public Visitor<bool> {

  public:
  int interpret(Node<bool>& root, vector<int> args, map<string, int>* variablesMap);

  virtual void visitAdditionExpression(AdditionExpression<bool>& expr);
  virtual void visitSubtractionExpression(SubtractionExpression<bool>& expr);
  virtual void visitMultiplicationExpression(MultiplicationExpression<bool>& expr);
  virtual void visitDivisionExpression(DivisionExpression<bool>& expr);
  virtual void visitIntegerExpression(IntegerExpression<bool>& expr);
  virtual void visitVariableExpression(VariableExpression<bool>& expr);

  stack_with_container<int> stack;
  map<string, int>* variablesMap;
};

}}

#endif
