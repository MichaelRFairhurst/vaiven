#ifndef VAIVEN_AST_HEADER_VISITOR
#define VAIVEN_AST_HEADER_VISITOR

namespace vaiven { namespace ast {

template<typename RD>
class Node;
template<typename RD>
class AdditionExpression;
template<typename RD>
class SubtractionExpression;
template<typename RD>
class MultiplicationExpression;
template<typename RD>
class DivisionExpression;
template<typename RD>
class IntegerExpression;
template<typename RD>
class VariableExpression;
template<typename RD>
class ExpressionStatement;
template<typename RD>
class Block;
template<typename RD>
class FuncDecl;

template<typename RD=bool>
class Visitor {

  public:
  virtual void visitAdditionExpression(AdditionExpression<RD>& expr)=0;
  virtual void visitSubtractionExpression(SubtractionExpression<RD>& expr)=0;
  virtual void visitMultiplicationExpression(MultiplicationExpression<RD>& expr)=0;
  virtual void visitDivisionExpression(DivisionExpression<RD>& expr)=0;
  virtual void visitIntegerExpression(IntegerExpression<RD>& expr)=0;
  virtual void visitVariableExpression(VariableExpression<RD>& expr)=0;
  virtual void visitExpressionStatement(ExpressionStatement<RD>& stmt)=0;
  virtual void visitBlock(Block<RD>& block)=0;
  virtual void visitFuncDecl(FuncDecl<RD>& funcDecl)=0;

};

}}

#endif
