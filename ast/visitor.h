#ifndef VAIVEN_AST_HEADER_VISITOR
#define VAIVEN_AST_HEADER_VISITOR

namespace vaiven { namespace ast {

template<typename RD=bool>
class Node;
template<typename RD=bool>
class AdditionExpression;
template<typename RD=bool>
class SubtractionExpression;
template<typename RD=bool>
class MultiplicationExpression;
template<typename RD=bool>
class DivisionExpression;
template<typename RD=bool>
class IntegerExpression;
template<typename RD=bool>
class VariableExpression;
template<typename RD=bool>
class ExpressionStatement;
template<typename RD=bool>
class Block;
template<typename RD=bool>
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
