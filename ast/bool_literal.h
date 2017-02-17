#ifndef VAIVEN_AST_HEADER_BOOL_LITERAL
#define VAIVEN_AST_HEADER_BOOL_LITERAL

#include <memory>
#include "expression.h"

using std::unique_ptr;

namespace vaiven { namespace ast {

template<typename RD>
class BoolLiteral : public Expression<RD> {

  public:
  BoolLiteral(bool value) : value(value) {};

  void accept(Visitor<RD>& v) {
    return v.visitBoolLiteral(*this);
  }
  virtual ~BoolLiteral() {};

  bool value;
};

}}

#endif
