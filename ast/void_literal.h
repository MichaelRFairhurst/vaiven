#ifndef VAIVEN_AST_HEADER_VOID_LITERAL
#define VAIVEN_AST_HEADER_VOID_LITERAL

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class VoidLiteral : public Expression<RD> {

  public:
  VoidLiteral() {}

  void accept(Visitor<RD>& v) {
    return v.visitVoidLiteral(*this);
  }
  virtual ~VoidLiteral() {};
};

}}

#endif
