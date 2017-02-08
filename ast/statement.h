#ifndef VAIVEN_AST_HEADER_STATEMENT
#define VAIVEN_AST_HEADER_STATEMENT

#include "node.h"

namespace vaiven { namespace ast {

// nothing yet
template<typename RD=void>
class Statement : public Node<RD> {
  public:
  virtual ~Statement() {};
};

}}

#endif
