#ifndef VAIVEN_AST_HEADER_NODE
#define VAIVEN_AST_HEADER_NODE

#include "visitor.h"

namespace vaiven { namespace ast {

template<typename RD>
class Node {
  public:
  virtual void accept(Visitor<RD>& v)=0;
  virtual ~Node() {};

  RD resolvedData;
};

}}

#endif
