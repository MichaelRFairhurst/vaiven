#ifndef VAIVEN_AST_HEADER_BLOCK
#define VAIVEN_AST_HEADER_BLOCK

#include <memory>
#include <vector>
#include "statement.h"

using std::unique_ptr;
using std::vector;

namespace vaiven { namespace ast {

template<typename RD>
class Block : public Statement<RD> {

  public:
  Block(
      vector<unique_ptr<Statement<RD> > > statements)
      : statements(std::move(statements)) {};

  void accept(Visitor<RD>& v) {
    return v.visitBlock(*this);
  }
  virtual ~Block() {};

  vector<unique_ptr<Statement<RD> > > statements;
};

}}

#endif
