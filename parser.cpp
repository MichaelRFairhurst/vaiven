#include "parser.h"

#include "ast/all.h"

#include <vector>
using std::vector;

using namespace vaiven;

unique_ptr<ast::Node<> > Parser::parseLogicalGroup() {
  if (current->type == TOKEN_TYPE_CLOSE_BRACE) next(); // for next block after {}
  if (current->type == TOKEN_TYPE_END) next(); // for next block after fn..end
  while (current->type == TOKEN_TYPE_SEMICOLON) next(); // consume semicolon

  if (current->type == TOKEN_TYPE_FN) {
    lastLogicalGroupWasEvaluatable = false;
    return parseFuncDecl();
  }

  lastLogicalGroupWasEvaluatable = true;
  unique_ptr<ast::Statement<> > expr = parseStatement();
  return unique_ptr<ast::Node<> >(expr.release());
}

unique_ptr<ast::FuncDecl<> > Parser::parseFuncDecl() {
  if (current->type != TOKEN_TYPE_FN) {
    throw string("expected fn");
  }
  next();
  if(current->type != TOKEN_TYPE_ID) {
    throw string("expected function name");
  }
  unique_ptr<StringToken> nametok(static_cast<StringToken*>(current.release()));
  string name = nametok->lexeme;
  next();
  vector<string> args;
  if (current->type == TOKEN_TYPE_OF) {
    next();
    while(current->type == TOKEN_TYPE_ID) {
      unique_ptr<StringToken> idtok(static_cast<StringToken*>(current.release()));
      next();
      args.push_back(idtok->lexeme);
      if (current->type != TOKEN_TYPE_COMMA) {
        break;
      }
      next();
    }
  }

  if (current->type != TOKEN_TYPE_IS) {
    throw string("expected is");
  }

  next();
  vector<unique_ptr<ast::Statement<> > > stmts;
  while (current->type != TOKEN_TYPE_EOF
      && current->type != TOKEN_TYPE_END) {
    stmts.push_back(parseStatement());
    next(); // eat semicolon
  }

  if (current->type != TOKEN_TYPE_END) {
    throw string("missing close brace");
  }

  return unique_ptr<ast::FuncDecl<> >(new ast::FuncDecl<>(name, args, std::move(stmts)));
}

unique_ptr<ast::Statement<> > Parser::parseStatement() {
  if (current->type == TOKEN_TYPE_OPEN_BRACE) {
    next();
    vector<unique_ptr<ast::Statement<> > > stmts;
    while (current->type != TOKEN_TYPE_EOF
        && current->type != TOKEN_TYPE_CLOSE_BRACE) {
      stmts.push_back(parseStatement());

      if (current->type != TOKEN_TYPE_SEMICOLON) {
        throw "expected ;";
      }
    }

    if (current->type != TOKEN_TYPE_CLOSE_BRACE) {
      throw string("missing close brace");
    }


    return unique_ptr<ast::Statement<> >(new ast::Block<>(std::move(stmts)));
  } else if (current->type == TOKEN_TYPE_VAR) {
    next();
    if(current->type != TOKEN_TYPE_ID) {
      throw string("expected var name");
    }
    unique_ptr<StringToken> nametok(static_cast<StringToken*>(current.release()));
    string name = nametok->lexeme;
    next();
    if(current->type != TOKEN_TYPE_EQ) {
      throw string("expected =");
    }
    next();
    unique_ptr<ast::Expression<> > initializer = parseExpression();
    if (current->type != TOKEN_TYPE_SEMICOLON) {
      throw string("missing end semicolon");
    }

    return unique_ptr<ast::Statement<> >(new ast::VarDecl<>(name, std::move(initializer)));
  }
    
    

  unique_ptr<ast::Statement<> > stmt(new ast::ExpressionStatement<>(parseExpression()));

  if (current->type != TOKEN_TYPE_SEMICOLON) {
    throw string("missing end semicolon");
  }
  
  return std::move(stmt);
}

unique_ptr<ast::Expression<> > Parser::parseExpression() {
  unique_ptr<ast::Expression<> > lhs(parseAddSubExpression());
  return std::move(lhs);
}

unique_ptr<ast::Expression<> > Parser::parseAddSubExpression() {
  unique_ptr<ast::Expression<> > acc(parseDivMulExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_PLUS) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::AdditionExpression<> (
          std::move(acc), parseDivMulExpression()));
    } else if (current->type == TOKEN_TYPE_MINUS) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::SubtractionExpression<> (
          std::move(acc), parseDivMulExpression()));
    } else {
      // error
      throw string("blah");
    }
  }

  return std::move(acc);

}

unique_ptr<ast::Expression<> > Parser::parseDivMulExpression() {
  unique_ptr<ast::Expression<> > acc(parseValue());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_PLUS
      && current->type != TOKEN_TYPE_MINUS
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_EOF) {

    if (current->type == TOKEN_TYPE_DIVIDE) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::DivisionExpression<> (
          std::move(acc), parseValue()));
    } else if (current->type == TOKEN_TYPE_MULTIPLY) {
      next();
      acc = unique_ptr<ast::Expression<> >(new ast::MultiplicationExpression<> (
          std::move(acc), parseValue()));
    } else {
      // error
      throw string("blah");
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<> > Parser::parseValue() {
  if (current->type == TOKEN_TYPE_INTEGER) {
    unique_ptr<IntegerToken> inttok(static_cast<IntegerToken*>(current.release()));
    next();
    return unique_ptr<ast::Expression<> >(new ast::IntegerExpression<> (inttok->value));
  }

  if (current->type == TOKEN_TYPE_OPEN_PAREN) {
    next();
    unique_ptr<ast::Expression<> > expr(parseExpression());
    // TODO check it actually is TYPE_CLOSE_PAREN
    next();
    return std::move(expr);
  }

  if (current->type == TOKEN_TYPE_ID) {
    unique_ptr<StringToken> idtok(static_cast<StringToken*>(current.release()));
    next();
    if (current->type == TOKEN_TYPE_OPEN_PAREN) {
      next();
      vector<unique_ptr<ast::Expression<> > > params;
      while (current->type != TOKEN_TYPE_EOF
          && current->type != TOKEN_TYPE_CLOSE_PAREN) {
        params.push_back(parseExpression());

        if (current->type != TOKEN_TYPE_COMMA) {
          break;
        }

        next();
      }

      if (current->type != TOKEN_TYPE_CLOSE_PAREN) {
        throw string("missing close paren");
      }

      next();
      return unique_ptr<ast::Expression<> >(new ast::FuncCallExpression<> (idtok->lexeme, move(params)));
    } else {
      return unique_ptr<ast::Expression<> >(new ast::VariableExpression<> (idtok->lexeme));
    }
  }
  
  throw string("blah");
}

void Parser::next() {
  current = tokenizer.next();
}

