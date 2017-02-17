#ifndef VAIVEN_HEADER_PARSER
#define VAIVEN_HEADER_PARSER

#include <memory>

#include "tokenizer.h"
#include "ast/all.h"

using std::unique_ptr;

namespace vaiven {

class Parser {
  public:
  Parser(Tokenizer& tokenizer) : tokenizer(tokenizer), current(tokenizer.next()) {}

  unique_ptr<ast::Node<> > parseLogicalGroup();

  unique_ptr<ast::FuncDecl<> > parseFuncDecl();
  unique_ptr<ast::Statement<> > parseStatement();
  unique_ptr<ast::Block<> > parseBlock();
  unique_ptr<ast::VarDecl<> > parseVarDecl();
  unique_ptr<ast::IfStatement<> > parseIfStatement();
  unique_ptr<ast::ReturnStatement<> > parseReturnStatement();
  unique_ptr<ast::ExpressionStatement<> > parseExpressionStatement();
  unique_ptr<ast::Expression<> > parseExpression();
  unique_ptr<ast::Expression<> > parseEqualityExpression();
  unique_ptr<ast::Expression<> > parseComparisonExpression();
  unique_ptr<ast::Expression<> > parseAddSubExpression();
  unique_ptr<ast::Expression<> > parseDivMulExpression();
  unique_ptr<ast::Expression<> > parseValue();

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
