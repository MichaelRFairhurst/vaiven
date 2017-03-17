#ifndef VAIVEN_AST_HEADER_STRING_EXPRESSION
#define VAIVEN_AST_HEADER_STRING_EXPRESSION

#include <memory>
#include "expression.h"
#include "../heap.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class StringExpression : public Expression<RD> {

  public:
  StringExpression(GcableString* value) : value(value) {};
  virtual ~StringExpression() {
    if (globalHeap == NULL) {
      delete value;
    } else {
      globalHeap->owned_ptrs.insert(value);
    }
  }

  void accept(Visitor<RD>& v) {
    return v.visitStringExpression(*this);
  }

  GcableString* value;
};

}}

#endif
