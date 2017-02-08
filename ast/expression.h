#ifndef VAIVEN_AST_HEADER_EXPRESSION
#define VAIVEN_AST_HEADER_EXPRESSION

#include "node.h"

namespace vaiven { namespace ast {

// nothing yet
template<typename RD=void>
class Expression : public Node<RD> {
  public:
  virtual ~Expression() {};
};

}}

#endif
