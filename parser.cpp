#include "parser.h"

#include "ast/all.h"

using namespace vaiven;

unique_ptr<ast::Node<bool> > Parser::nextEvaluatableBlock() {
  while (current->type == TOKEN_TYPE_SEMICOLON) next(); // consume semicolon
  unique_ptr<ast::Expression<bool> > expr = parseExpression();
  return unique_ptr<ast::Node<bool> >(expr.release());
}

unique_ptr<ast::Expression<bool> > Parser::parseExpression() {
  unique_ptr<ast::Expression<bool> > lhs(parseAddSubExpression());
  return std::move(lhs);
}

unique_ptr<ast::Expression<bool> > Parser::parseAddSubExpression() {
  unique_ptr<ast::Expression<bool> > acc(parseDivMulExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_PLUS) {
      next();
      acc = unique_ptr<ast::Expression<bool> >(new ast::AdditionExpression<bool> (
          std::move(acc), parseDivMulExpression()));
    } else if (current->type == TOKEN_TYPE_MINUS) {
      next();
      acc = unique_ptr<ast::Expression<bool> >(new ast::SubtractionExpression<bool> (
          std::move(acc), parseDivMulExpression()));
    } else {
      // error
      return unique_ptr<ast::Expression<bool> >(NULL);
    }
  }

  return std::move(acc);

}

unique_ptr<ast::Expression<bool> > Parser::parseDivMulExpression() {
  unique_ptr<ast::Expression<bool> > acc(parseValue());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_PLUS
      && current->type != TOKEN_TYPE_MINUS
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_DIVIDE) {
      next();
      acc = unique_ptr<ast::Expression<bool> >(new ast::DivisionExpression<bool> (
          std::move(acc), parseValue()));
    } else if (current->type == TOKEN_TYPE_MULTIPLY) {
      next();
      acc = unique_ptr<ast::Expression<bool> >(new ast::MultiplicationExpression<bool> (
          std::move(acc), parseValue()));
    } else {
      // error
      return unique_ptr<ast::Expression<bool> >(NULL);
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<bool> > Parser::parseValue() {
  if (current->type == TOKEN_TYPE_INTEGER) {
    unique_ptr<IntegerToken> inttok(static_cast<IntegerToken*>(current.release()));
    next();
    return unique_ptr<ast::Expression<bool> >(new ast::IntegerExpression<bool> (inttok->value));
  }

  if (current->type == TOKEN_TYPE_OPEN_PAREN) {
    next();
    unique_ptr<ast::Expression<bool> > expr(parseExpression());
    // TODO check it actually is TYPE_CLOSE_PAREN
    next();
    return std::move(expr);
  }

  if (current->type == TOKEN_TYPE_ID) {
    unique_ptr<StringToken> idtok(static_cast<StringToken*>(current.release()));
    next();
    return unique_ptr<ast::Expression<bool> >(new ast::VariableExpression<bool> (idtok->lexeme));
  }
  
  return unique_ptr<ast::Expression<bool> >(NULL);
}

void Parser::next() {
  current = tokenizer.next();
}

