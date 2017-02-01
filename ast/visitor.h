#ifndef VAIVEN_AST_HEADER_VISITOR
#define VAIVEN_AST_HEADER_VISITOR

namespace vaiven { namespace ast {

template<typename ResolvedData>
class Node;
template<typename ResolvedData>
class AdditionExpression;
template<typename ResolvedData>
class SubtractionExpression;
template<typename ResolvedData>
class MultiplicationExpression;
template<typename ResolvedData>
class DivisionExpression;
template<typename ResolvedData>
class IntegerExpression;

template<typename ResolvedData>
class Visitor {

  public:
  virtual void visitAdditionExpression(AdditionExpression<ResolvedData>& expr)=0;
  virtual void visitSubtractionExpression(SubtractionExpression<ResolvedData>& expr)=0;
  virtual void visitMultiplicationExpression(MultiplicationExpression<ResolvedData>& expr)=0;
  virtual void visitDivisionExpression(DivisionExpression<ResolvedData>& expr)=0;
  virtual void visitIntegerExpression(IntegerExpression<ResolvedData>& expr)=0;

};

}}

#endif
