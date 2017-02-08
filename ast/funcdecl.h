#ifndef VAIVEN_AST_HEADER_FUNCDECL
#define VAIVEN_AST_HEADER_FUNCDECL

#include <memory>
#include <vector>
#include <string>
#include "statement.h"

using std::unique_ptr;
using std::vector;
using std::string;

namespace vaiven { namespace ast {

template<typename RD=bool>
class FuncDecl : public Node<RD> {

  public:
  FuncDecl(
      string name,
      vector<string> args,
      vector<unique_ptr<Statement<RD> > > statements)
      : statements(std::move(statements)), name(name), args(args) {};

  void accept(Visitor<RD>& v) {
    return v.visitFuncDecl(*this);
  }
  virtual ~FuncDecl() {};

  string name;
  vector<unique_ptr<Statement<RD> > > statements;
  vector<string> args;
};

}}

#endif
