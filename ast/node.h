#ifndef VAIVEN_AST_HEADER_NODE
#define VAIVEN_AST_HEADER_NODE

#include "visitor.h"

namespace vaiven { namespace ast {

template<typename ResolvedData=void>
class Node {
  public:
  virtual void accept(Visitor<ResolvedData>& v)=0;
  virtual ~Node() {};

  ResolvedData resolvedData;
};

}}

#endif
