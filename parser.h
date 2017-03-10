#ifndef VAIVEN_HEADER_PARSER
#define VAIVEN_HEADER_PARSER

#include <memory>
#include <string>

#include "tokenizer.h"
#include "ast/all.h"

using std::unique_ptr;
using std::string;

namespace vaiven {

class ParseError;

class Parser {
  public:
  Parser(Tokenizer& tokenizer)
    : tokenizer(tokenizer), current(tokenizer.next()), newlineReplacement(TOKEN_TYPE_SEMICOLON),
    errorLocation("top level"), ignoreNextNext(false) {}

  unique_ptr<ast::Node<> > parseLogicalGroup();

  unique_ptr<ast::FuncDecl<> > parseFuncDecl();
  unique_ptr<ast::Statement<> > parseStatement();
  unique_ptr<ast::Block<> > parseBlock();
  unique_ptr<ast::VarDecl<> > parseVarDecl();
  unique_ptr<ast::IfStatement<> > parseIfStatement();
  unique_ptr<ast::ForCondition<> > parseForCondition();
  unique_ptr<ast::ReturnStatement<> > parseReturnStatement();
  unique_ptr<ast::ExpressionStatement<> > parseExpressionStatement();
  unique_ptr<ast::Expression<> > parseExpression();
  unique_ptr<ast::Expression<> > parseAssignmentExpression();
  unique_ptr<ast::Expression<> > parseEqualityExpression();
  unique_ptr<ast::Expression<> > parseComparisonExpression();
  unique_ptr<ast::Expression<> > parseAddSubExpression();
  unique_ptr<ast::Expression<> > parseDivMulExpression();
  unique_ptr<ast::Expression<> > parseValue();

  // Store this on the parser rather than using a visitor
  // to keep interpreter workflow fast
  bool lastLogicalGroupWasEvaluatable;

  vector<ParseError> errors;

  private:
  bool ignoreNextNext;
  void next();
  void nextNoEol();
  void nextOr(TokenType newlineType);

  Tokenizer& tokenizer;
  unique_ptr<Token> current;
  TokenType newlineReplacement;

  string errorLocation;
};

class ParseError {
  public:
  string error;
  string location;

  ParseError(string error, string location) : error(error), location(location) {}
};

}

#endif
