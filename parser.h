#ifndef VAIVEN_HEADER_PARSER
#define VAIVEN_HEADER_PARSER

#include <memory>

#include "tokenizer.h"
#include "ast/expression.h"
#include "ast/statement.h"

using std::unique_ptr;

namespace vaiven {

class Parser {
  public:
  Parser(Tokenizer& tokenizer) : tokenizer(tokenizer), current(tokenizer.next()) {}

  unique_ptr<ast::Node<bool> > parseLogicalGroup();

  unique_ptr<ast::FuncDecl<bool> > parseFuncDecl();
  unique_ptr<ast::Statement<bool> > parseStatement();
  unique_ptr<ast::Expression<bool> > parseExpression();
  unique_ptr<ast::Expression<bool> > parseAddSubExpression();
  unique_ptr<ast::Expression<bool> > parseDivMulExpression();
  unique_ptr<ast::Expression<bool> > parseValue();

  // Store this on the parser rather than using a visitor
  // to keep interpreter workflow fast
  bool lastLogicalGroupWasEvaluatable;

  private:
  void next();

  Tokenizer& tokenizer;
  unique_ptr<Token> current;
};

}

#endif
