#ifndef VAIVEN_AST_HEADER_VISITOR
#define VAIVEN_AST_HEADER_VISITOR

namespace vaiven { namespace ast {

template<typename RD=void>
class Node;
template<typename RD=void>
class AdditionExpression;
template<typename RD=void>
class SubtractionExpression;
template<typename RD=void>
class MultiplicationExpression;
template<typename RD=void>
class DivisionExpression;
template<typename RD=void>
class IntegerExpression;
template<typename RD=void>
class VariableExpression;
template<typename RD=void>
class ExpressionStatement;
template<typename RD=void>
class Block;
template<typename RD=void>
class FuncDecl;
template<typename RD=void>
class FuncCallExpression;
template<typename RD=void>
class VarDecl;

template<typename RD=void>
class Visitor {

  public:
  virtual void visitAdditionExpression(AdditionExpression<RD>& expr)=0;
  virtual void visitSubtractionExpression(SubtractionExpression<RD>& expr)=0;
  virtual void visitMultiplicationExpression(MultiplicationExpression<RD>& expr)=0;
  virtual void visitDivisionExpression(DivisionExpression<RD>& expr)=0;
  virtual void visitIntegerExpression(IntegerExpression<RD>& expr)=0;
  virtual void visitVariableExpression(VariableExpression<RD>& expr)=0;
  virtual void visitFuncCallExpression(FuncCallExpression<RD>& expr)=0;
  virtual void visitExpressionStatement(ExpressionStatement<RD>& stmt)=0;
  virtual void visitBlock(Block<RD>& block)=0;
  virtual void visitFuncDecl(FuncDecl<RD>& funcDecl)=0;
  virtual void visitVarDecl(VarDecl<RD>& varDecl)=0;

};

}}

#endif
