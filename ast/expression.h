#ifndef VAIVEN_AST_HEADER_EXPRESSION
#define VAIVEN_AST_HEADER_EXPRESSION

#include "node.h"

namespace vaiven { namespace ast {

// nothing yet
template<typename ResolvedData=void>
class Expression : public Node<ResolvedData> {
  public:
  void accept(Visitor<ResolvedData>& v)=0;
  virtual ~Expression() {};
};

}}

#endif
