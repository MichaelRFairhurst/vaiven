#include "parser.h"

#include "ast/all.h"
#include "visitor/assignment_producer.h"

#include <vector>
using std::vector;
using vaiven::visitor::AssignmentProducer;

using namespace vaiven;

unique_ptr<ast::Node<> > Parser::parseLogicalGroup() {
  if (current->type == TOKEN_TYPE_EOF) {
    return unique_ptr<ast::Node<> >((ast::Node<>*) NULL);
  }

  if (current->type == TOKEN_TYPE_CLOSE_BRACE) next(); // for next block after {}
  if (current->type == TOKEN_TYPE_END) next(); // for next block after fn..end
  while (current->type == TOKEN_TYPE_SEMICOLON) next(); // consume semicolon

  // after next();
  if (current->type == TOKEN_TYPE_EOF) {
    return unique_ptr<ast::Node<> >((ast::Node<>*) NULL);
  }

  try {
    if (current->type == TOKEN_TYPE_FN) {
      lastLogicalGroupWasEvaluatable = false;
      return parseFuncDecl();
    }

    lastLogicalGroupWasEvaluatable = true;
    unique_ptr<ast::Statement<> > expr = parseStatement();
    return unique_ptr<ast::Node<> >(expr.release());
  } catch(ParseError e) {
    errors.push_back(e);
    next();
    return unique_ptr<ast::Node<> >((ast::Node<>*) NULL);
  }
}

unique_ptr<ast::FuncDecl<> > Parser::parseFuncDecl() {
  if (current->type != TOKEN_TYPE_FN) {
    // should never happen
    throw string("expected fn");
  }
  nextNoEol();

  string name = "";
  if(current->type == TOKEN_TYPE_ID) {
    unique_ptr<StringToken> nametok(static_cast<StringToken*>(current->copy()));
    name = nametok->lexeme;
    errorLocation = "function " + name;
    nextOr(TOKEN_TYPE_IS);
  } else {
    errorLocation = "function without a name";
  }

  vector<string> args;
  if (current->type == TOKEN_TYPE_OF) {
    nextNoEol();
    while(current->type == TOKEN_TYPE_ID) {
      unique_ptr<StringToken> idtok(static_cast<StringToken*>(current->copy()));
      nextOr(TOKEN_TYPE_COMMA);
      args.push_back(idtok->lexeme);
      if (current->type != TOKEN_TYPE_COMMA) {
        break;
      }
      nextNoEol();
    }
  }

  if (current->type == TOKEN_TYPE_IS) {
    nextNoEol();
  } else {
    errors.push_back(ParseError("missing is at end of function", errorLocation));
  }

  vector<unique_ptr<ast::Statement<> > > stmts;
  while (current->type != TOKEN_TYPE_END && current->type != TOKEN_TYPE_EOF) {
    try {
      stmts.push_back(parseStatement());
    } catch(ParseError e) {
      errors.push_back(e);
    }
    nextNoEol(); // statements don't consume their final token
  }

  if (current->type == TOKEN_TYPE_EOF) {
    errors.push_back(ParseError("Unexpected EOF", errorLocation));
  }

  errorLocation = "top level";
  if (name == "") {
    throw ParseError("missing function name", "function declaration");
  }
  return unique_ptr<ast::FuncDecl<> >(new ast::FuncDecl<>(name, args, std::move(stmts)));
}

unique_ptr<ast::Statement<> > Parser::parseStatement() {
  if (current->type == TOKEN_TYPE_OPEN_BRACE
      || current->type == TOKEN_TYPE_VAR
      || current->type == TOKEN_TYPE_IF
      || current->type == TOKEN_TYPE_FOR
      || current->type == TOKEN_TYPE_RET) {
    return parseControlStatement();
  } else {
    return unique_ptr<ast::Statement<> > (parseExpressionStatement().release());
  }
}

unique_ptr<ast::Statement<> > Parser::parseControlStatement() {
  if (current->type == TOKEN_TYPE_OPEN_BRACE) {
    return unique_ptr<ast::Statement<> > (parseBlock().release());
  } else if (current->type == TOKEN_TYPE_VAR) {
    return unique_ptr<ast::Statement<> > (parseVarDecl().release());
  } else if (current->type == TOKEN_TYPE_IF) {
    return unique_ptr<ast::Statement<> > (parseIfStatement().release());
  } else if (current->type == TOKEN_TYPE_FOR) {
    return unique_ptr<ast::Statement<> > (parseForCondition().release());
  } else if (current->type == TOKEN_TYPE_RET) {
    return unique_ptr<ast::Statement<> > (parseReturnStatement().release());
  }

  throw ParseError("Expected control statement but did not get a control keyword", errorLocation);
}

unique_ptr<ast::VarDecl<> > Parser::parseVarDecl() {
  nextNoEol();
  string name = "";
  if(current->type == TOKEN_TYPE_ID) {
    unique_ptr<StringToken> nametok(static_cast<StringToken*>(current->copy()));
    name = nametok->lexeme;
    nextNoEol();
  }
  if(current->type == TOKEN_TYPE_EQ) {
    nextNoEol();
  } else {
    errors.push_back(ParseError("no '=' after var declaration", errorLocation));
  }

  unique_ptr<ast::Expression<> > initializer = parseExpression();
  if (current->type == TOKEN_TYPE_END || current->type == TOKEN_TYPE_ELSE) {
    ignoreNextNext = true; // end will be consumed otherwise after this
  } else if (current->type != TOKEN_TYPE_SEMICOLON) {
    errors.push_back(ParseError("missing semicolon after var declaration", errorLocation));
    ignoreNextNext = true; // the error token will be consumed by the container without this
  }

  if (name == "") {
    throw ParseError("missing var name in declaration", errorLocation);
  }
  return unique_ptr<ast::VarDecl<> >(new ast::VarDecl<>(name, std::move(initializer)));
}

unique_ptr<ast::Block<> > Parser::parseBlock() {
  next();
  vector<unique_ptr<ast::Statement<> > > stmts;
  while (current->type != TOKEN_TYPE_EOF
      && current->type != TOKEN_TYPE_CLOSE_BRACE) {
    try {
      stmts.push_back(parseStatement());
    } catch(ParseError e) {
      errors.push_back(e);
    }

    nextNoEol(); // statements don't consume their final token
  }

  if (current->type != TOKEN_TYPE_CLOSE_BRACE) {
    errors.push_back(ParseError("missing 'end' at end of block", errorLocation));
  }

  return unique_ptr<ast::Block<> >(new ast::Block<>(std::move(stmts)));
}

unique_ptr<ast::IfStatement<> > Parser::parseIfStatement() {
  next();
  newlineReplacement = TOKEN_TYPE_DO;
  unique_ptr<ast::Expression<> > condition;
  try {
    condition = parseExpression();
    newlineReplacement = TOKEN_TYPE_SEMICOLON;
  } catch(ParseError e) {
    newlineReplacement = TOKEN_TYPE_SEMICOLON;
    throw e;
  }

  vector<unique_ptr<ast::Statement<> > > trueStmts;
  vector<unique_ptr<ast::Statement<> > > falseStmts;
  if (current->type != TOKEN_TYPE_DO) {
    try {
      trueStmts.push_back(parseControlStatement());
    } catch(ParseError e) {
      errors.push_back(e);
    }
  } else {
    nextNoEol();
    while (current->type != TOKEN_TYPE_EOF
        && current->type != TOKEN_TYPE_ELSE
        && current->type != TOKEN_TYPE_END) {
      try {
        trueStmts.push_back(parseStatement());
      } catch(ParseError e) {
        errors.push_back(e);
      }

      nextNoEol(); // statements don't consume their final token
    }
  }

  if (current->type == TOKEN_TYPE_ELSE) {
    nextOr(TOKEN_TYPE_DO);
    if (current->type != TOKEN_TYPE_DO) {
      try {
        falseStmts.push_back(parseControlStatement());
      } catch(ParseError e) {
        errors.push_back(e);
      }
    } else {
      nextNoEol();
      while (current->type != TOKEN_TYPE_EOF
          && current->type != TOKEN_TYPE_END) {
        try {
          falseStmts.push_back(parseStatement());
        } catch(ParseError e) {
          errors.push_back(e);
        }

        nextNoEol(); // statements don't consume their final token
      }
      if (current->type != TOKEN_TYPE_END) {
        errors.push_back(ParseError("missing 'end' at end of if", errorLocation));
      }
    }
  } else if (current->type != TOKEN_TYPE_END) {
    errors.push_back(ParseError("missing 'end' or 'else' at end of if", errorLocation));
  }

  return unique_ptr<ast::IfStatement<> >(new ast::IfStatement<>(
      std::move(condition),
      std::move(trueStmts),
      std::move(falseStmts)));
}

unique_ptr<ast::ForCondition<> > Parser::parseForCondition() {
  next();
  newlineReplacement = TOKEN_TYPE_DO;
  unique_ptr<ast::Expression<> > condition;
  try {
    condition = parseExpression();
    newlineReplacement = TOKEN_TYPE_SEMICOLON;
  } catch(ParseError e) {
    newlineReplacement = TOKEN_TYPE_SEMICOLON;
    throw e;
  }

  vector<unique_ptr<ast::Statement<> > > stmts;
  if (current->type != TOKEN_TYPE_DO) {
    try {
      stmts.push_back(parseControlStatement());
    } catch(ParseError e) {
      errors.push_back(e);
    }
  } else {
    nextNoEol();
    while (current->type != TOKEN_TYPE_EOF
        && current->type != TOKEN_TYPE_END) {
      try {
        stmts.push_back(parseStatement());
      } catch(ParseError e) {
        errors.push_back(e);
      }

      nextNoEol(); // statements don't consume their final token
    }
  }

  if (current->type != TOKEN_TYPE_END) {
    errors.push_back(ParseError("missing 'end' at end of for block", errorLocation));
  }

  return unique_ptr<ast::ForCondition<> >(new ast::ForCondition<>(
      std::move(condition),
      std::move(stmts)));
}

unique_ptr<ast::ReturnStatement<> > Parser::parseReturnStatement() {
  next();
  unique_ptr<ast::ReturnStatement<> > stmt(new ast::ReturnStatement<>(parseExpression()));

  if (current->type == TOKEN_TYPE_END || current->type == TOKEN_TYPE_ELSE) {
    ignoreNextNext = true; // end will be consumed otherwise after this
  } else if (current->type != TOKEN_TYPE_SEMICOLON) {
    errors.push_back(ParseError("missing semicolon (or newline) at end of return", errorLocation));
  }
  
  return std::move(stmt);
}

unique_ptr<ast::ExpressionStatement<> > Parser::parseExpressionStatement() {
  unique_ptr<ast::ExpressionStatement<> > stmt(new ast::ExpressionStatement<>(parseExpression()));

  if (current->type == TOKEN_TYPE_END || current->type == TOKEN_TYPE_ELSE) {
    ignoreNextNext = true; // end will be consumed otherwise after this
  } else if (current->type != TOKEN_TYPE_SEMICOLON) {
    errors.push_back(ParseError("missing semicolon (or newline) at end of expression", errorLocation));
  }
  
  return std::move(stmt);
}


unique_ptr<ast::Expression<> > Parser::parseExpression() {
  // special case: ) as a value doesn't consume. Therefore we have to consume here
  while (true) {
    if (current->type == TOKEN_TYPE_CLOSE_PAREN) {
      errors.push_back(ParseError("expected expression, got a close paren", errorLocation));
      next();
    } else if (current->type == TOKEN_TYPE_CLOSE_BRACKET) {
      errors.push_back(ParseError("expected expression, got a close bracket", errorLocation));
      next();
    } else {
      break;
    }
  }

  unique_ptr<ast::Expression<> > lhs(parseAssignmentExpression());
  return std::move(lhs);
}

unique_ptr<ast::Expression<> > Parser::parseAssignmentExpression() {
  unique_ptr<ast::Expression<> > lhs(parseEqualityExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_CLOSE_BRACKET
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EOF) {

    ast::PreAssignmentOp preAssignmentOp;
    if (current->type == TOKEN_TYPE_EQ) {
      preAssignmentOp = ast::kPreAssignmentOpNone;
    } else if (current->type == TOKEN_TYPE_PLUSEQ) {
      preAssignmentOp = ast::kPreAssignmentOpAdd;
    } else if (current->type == TOKEN_TYPE_SUBEQ) {
      preAssignmentOp = ast::kPreAssignmentOpSub;
    } else if (current->type == TOKEN_TYPE_MULEQ) {
      preAssignmentOp = ast::kPreAssignmentOpMul;
    } else if (current->type == TOKEN_TYPE_DIVEQ) {
      preAssignmentOp = ast::kPreAssignmentOpDiv;
    } else {
      // error to be caught later likely
      break;
    }

    nextNoEol();
    try {
      unique_ptr<ast::Expression<> > rhs = parseAssignmentExpression();
      AssignmentProducer assignmentProducer(std::move(rhs), preAssignmentOp);
      lhs->accept(assignmentProducer);
      if (assignmentProducer.result.get() == NULL) {
        errors.push_back(ParseError("assignment to a nonassignable expression", errorLocation));
        break;
      }
      return std::move(assignmentProducer.result);
    } catch(ParseError e) {
      errors.push_back(e);
      return lhs;
    }
  }

  return std::move(lhs);
}

unique_ptr<ast::Expression<> > Parser::parseEqualityExpression() {
  unique_ptr<ast::Expression<> > acc(parseComparisonExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_CLOSE_BRACKET
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_PLUSEQ
      && current->type != TOKEN_TYPE_SUBEQ
      && current->type != TOKEN_TYPE_MULEQ
      && current->type != TOKEN_TYPE_DIVEQ
      && current->type != TOKEN_TYPE_EOF) {

    try {
      if (current->type == TOKEN_TYPE_EQEQ) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseComparisonExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::EqualityExpression<> (
            std::move(acc), std::move(rhs)));
      } else if (current->type == TOKEN_TYPE_BANGEQ) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseComparisonExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::InequalityExpression<> (
            std::move(acc), std::move(rhs)));
      } else {
        break;
      }
    } catch(ParseError e) {
      errors.push_back(e);
      break;
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<> > Parser::parseComparisonExpression() {
  unique_ptr<ast::Expression<> > acc(parseAddSubExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_CLOSE_BRACKET
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_PLUSEQ
      && current->type != TOKEN_TYPE_SUBEQ
      && current->type != TOKEN_TYPE_MULEQ
      && current->type != TOKEN_TYPE_DIVEQ
      && current->type != TOKEN_TYPE_EQEQ
      && current->type != TOKEN_TYPE_BANGEQ
      && current->type != TOKEN_TYPE_EOF) {

    try {
      if (current->type == TOKEN_TYPE_GT) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseAddSubExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::GtExpression<> (
            std::move(acc), std::move(rhs)));
      } else if (current->type == TOKEN_TYPE_GTE) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseAddSubExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::GteExpression<> (
            std::move(acc), std::move(rhs)));
      } else if (current->type == TOKEN_TYPE_LT) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseAddSubExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::LtExpression<> (
            std::move(acc), std::move(rhs)));
      } else if (current->type == TOKEN_TYPE_LTE) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseAddSubExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::LteExpression<> (
            std::move(acc), std::move(rhs)));
      } else {
        break;
      }
    } catch(ParseError e) {
      errors.push_back(e);
      break;
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<> > Parser::parseAddSubExpression() {
  unique_ptr<ast::Expression<> > acc(parseDivMulExpression());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_CLOSE_BRACKET
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_PLUSEQ
      && current->type != TOKEN_TYPE_SUBEQ
      && current->type != TOKEN_TYPE_MULEQ
      && current->type != TOKEN_TYPE_DIVEQ
      && current->type != TOKEN_TYPE_EQEQ
      && current->type != TOKEN_TYPE_BANGEQ
      && current->type != TOKEN_TYPE_GT
      && current->type != TOKEN_TYPE_GTE
      && current->type != TOKEN_TYPE_LT
      && current->type != TOKEN_TYPE_LTE
      && current->type != TOKEN_TYPE_EOF) {

    try {
      if (current->type == TOKEN_TYPE_PLUS) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseDivMulExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::AdditionExpression<> (
            std::move(acc), std::move(rhs)));
      } else if (current->type == TOKEN_TYPE_MINUS) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseDivMulExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::SubtractionExpression<> (
            std::move(acc), std::move(rhs)));
      } else {
        break;
      }
    } catch(ParseError e) {
      errors.push_back(e);
      //break;
    }
  }

  return std::move(acc);

}

unique_ptr<ast::Expression<> > Parser::parseDivMulExpression() {
  unique_ptr<ast::Expression<> > acc(parseAccess());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_PLUS
      && current->type != TOKEN_TYPE_MINUS
      && current->type != TOKEN_TYPE_MINUS
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_CLOSE_BRACKET
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_PLUSEQ
      && current->type != TOKEN_TYPE_SUBEQ
      && current->type != TOKEN_TYPE_MULEQ
      && current->type != TOKEN_TYPE_DIVEQ
      && current->type != TOKEN_TYPE_EQEQ
      && current->type != TOKEN_TYPE_BANGEQ
      && current->type != TOKEN_TYPE_GT
      && current->type != TOKEN_TYPE_GTE
      && current->type != TOKEN_TYPE_LT
      && current->type != TOKEN_TYPE_LTE
      && current->type != TOKEN_TYPE_EOF) {

    try {
      if (current->type == TOKEN_TYPE_DIVIDE) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseAccess());
        acc = unique_ptr<ast::Expression<> >(new ast::DivisionExpression<> (
            std::move(acc), std::move(rhs)));
      } else if (current->type == TOKEN_TYPE_MULTIPLY) {
        nextNoEol();
        unique_ptr<ast::Expression<> > rhs(parseAccess());
        acc = unique_ptr<ast::Expression<> >(new ast::MultiplicationExpression<> (
            std::move(acc), std::move(rhs)));
      } else {
        break;
      }
    } catch(ParseError e) {
      errors.push_back(e);
      break;
    }
  }

  return std::move(acc);
}

unique_ptr<ast::Expression<> > Parser::parseAccess() {
  unique_ptr<ast::Expression<> > acc(parseValue());

  while (current->type != TOKEN_TYPE_SEMICOLON
      && current->type != TOKEN_TYPE_PLUS
      && current->type != TOKEN_TYPE_MINUS
      && current->type != TOKEN_TYPE_DIVIDE
      && current->type != TOKEN_TYPE_MULTIPLY
      && current->type != TOKEN_TYPE_COMMA
      && current->type != TOKEN_TYPE_CLOSE_PAREN
      && current->type != TOKEN_TYPE_CLOSE_BRACKET
      && current->type != TOKEN_TYPE_DO
      && current->type != TOKEN_TYPE_EQ
      && current->type != TOKEN_TYPE_PLUSEQ
      && current->type != TOKEN_TYPE_SUBEQ
      && current->type != TOKEN_TYPE_MULEQ
      && current->type != TOKEN_TYPE_DIVEQ
      && current->type != TOKEN_TYPE_EQEQ
      && current->type != TOKEN_TYPE_BANGEQ
      && current->type != TOKEN_TYPE_GT
      && current->type != TOKEN_TYPE_GTE
      && current->type != TOKEN_TYPE_LT
      && current->type != TOKEN_TYPE_LTE
      && current->type != TOKEN_TYPE_EOF) {

    try {
      if (current->type == TOKEN_TYPE_OPEN_BRACKET) {
        nextNoEol();
        unique_ptr<ast::Expression<> > property(parseExpression());
        acc = unique_ptr<ast::Expression<> >(new ast::DynamicAccessExpression<> (
            std::move(acc), std::move(property)));
  
        if (current->type != TOKEN_TYPE_CLOSE_BRACKET) {
          errors.push_back(ParseError("missing close bracket in dynamic access", errorLocation));
          if (current->type == TOKEN_TYPE_END
            || current->type == TOKEN_TYPE_IF
            || current->type == TOKEN_TYPE_FOR
            || current->type == TOKEN_TYPE_ELSE) {
            ignoreNextNext = true;
          }
          break;
        }

        next();
      } else if (current->type == TOKEN_TYPE_DOT) {
        nextNoEol();
        if (current->type != TOKEN_TYPE_ID) {
          errors.push_back(ParseError("no token after . in property acces", errorLocation));
          if (current->type == TOKEN_TYPE_END
            || current->type == TOKEN_TYPE_IF
            || current->type == TOKEN_TYPE_FOR
            || current->type == TOKEN_TYPE_ELSE) {
            ignoreNextNext = true;
          }
          break;
        }
        unique_ptr<StringToken> idtok(static_cast<StringToken*>(current->copy()));

        next();

        if (current->type == TOKEN_TYPE_OPEN_PAREN) {
          nextNoEol();
          vector<unique_ptr<ast::Expression<> > > params;
          params.push_back(std::move(acc));
          while (current->type != TOKEN_TYPE_EOF
              && current->type != TOKEN_TYPE_CLOSE_PAREN) {
            TokenType prevNewlineReplacement = newlineReplacement;
            newlineReplacement = TOKEN_TYPE_COMMA;
            try {
              params.push_back(parseExpression());
              newlineReplacement = prevNewlineReplacement;
            } catch(ParseError e) {
              newlineReplacement = prevNewlineReplacement;
              throw e;
            }

            if (current->type != TOKEN_TYPE_COMMA) {
              break;
            }

            nextNoEol();
          }

          if (current->type != TOKEN_TYPE_CLOSE_PAREN) {
            errors.push_back(ParseError("missing close paren for function call", errorLocation));
          }

          next();
          acc = unique_ptr<ast::Expression<> >(new ast::FuncCallExpression<> (idtok->lexeme, move(params)));
        } else {
          acc = unique_ptr<ast::Expression<> >(new ast::StaticAccessExpression<> (
            std::move(acc), idtok->lexeme));
        }
      } else {
        break;
      }
    } catch(ParseError e) {
      errors.push_back(e);
      break;
    }
  }

  return acc;
}

unique_ptr<ast::Expression<> > Parser::parseValue() {
  if (current->type == TOKEN_TYPE_INTEGER) {
    unique_ptr<IntegerToken> inttok(static_cast<IntegerToken*>(current->copy()));
    next();
    return unique_ptr<ast::Expression<> >(new ast::IntegerExpression<>(inttok->value));
  }

  if (current->type == TOKEN_TYPE_DOUBLE) {
    unique_ptr<DoubleToken> inttok(static_cast<DoubleToken*>(current->copy()));
    next();
    return unique_ptr<ast::Expression<> >(new ast::DoubleExpression<>(inttok->value));
  }

  if (current->type == TOKEN_TYPE_STRING) {
    unique_ptr<StringToken> inttok(static_cast<StringToken*>(current->copy()));
    next();
    return unique_ptr<ast::Expression<> >(new ast::StringExpression<>(new GcableString(inttok->lexeme)));
  }

  if (current->type == TOKEN_TYPE_OPEN_PAREN) {
    TokenType prevNewlineReplacement = newlineReplacement;
    newlineReplacement = TOKEN_TYPE_ERROR; // no newline replacement
    nextNoEol();
    unique_ptr<ast::Expression<> > expr;
    try {
      expr = parseExpression();
      newlineReplacement = prevNewlineReplacement;
    } catch(ParseError e) {
      newlineReplacement = prevNewlineReplacement;
      throw e;
    }
    if (current->type != TOKEN_TYPE_CLOSE_PAREN) {
      errors.push_back(ParseError("missing close paren for subexpression", errorLocation));
    } else {
      next();
    }
    return std::move(expr);
  }

  if (current->type == TOKEN_TYPE_ID) {
    unique_ptr<StringToken> idtok(static_cast<StringToken*>(current->copy()));
    next();
    if (current->type == TOKEN_TYPE_OPEN_PAREN) {
      nextNoEol();
      vector<unique_ptr<ast::Expression<> > > params;
      while (current->type != TOKEN_TYPE_EOF
          && current->type != TOKEN_TYPE_CLOSE_PAREN) {
        TokenType prevNewlineReplacement = newlineReplacement;
        newlineReplacement = TOKEN_TYPE_COMMA;
        try {
          params.push_back(parseExpression());
          newlineReplacement = prevNewlineReplacement;
        } catch(ParseError e) {
          newlineReplacement = prevNewlineReplacement;
          throw e;
        }

        if (current->type != TOKEN_TYPE_COMMA) {
          break;
        }

        nextNoEol();
      }

      if (current->type != TOKEN_TYPE_CLOSE_PAREN) {
        errors.push_back(ParseError("missing close paren for function call", errorLocation));
      }

      next();
      return unique_ptr<ast::Expression<> >(new ast::FuncCallExpression<> (idtok->lexeme, move(params)));
    } else {
      return unique_ptr<ast::Expression<> >(new ast::VariableExpression<> (idtok->lexeme));
    }
  }

  if (current->type == TOKEN_TYPE_OPEN_BRACKET) {
    nextNoEol();
    vector<unique_ptr<ast::Expression<> > > items;
    while (current->type != TOKEN_TYPE_EOF
        && current->type != TOKEN_TYPE_CLOSE_BRACKET) {
      TokenType prevNewlineReplacement = newlineReplacement;
      newlineReplacement = TOKEN_TYPE_COMMA;
      try {
        items.push_back(parseExpression());
        newlineReplacement = prevNewlineReplacement;
      } catch(ParseError e) {
        newlineReplacement = prevNewlineReplacement;
        throw e;
      }

      if (current->type != TOKEN_TYPE_COMMA) {
        break;
      }

      nextNoEol();
    }

    if (current->type != TOKEN_TYPE_CLOSE_BRACKET) {
      errors.push_back(ParseError("missing close bracket for list literal", errorLocation));
    }

    next();
    return unique_ptr<ast::Expression<> >(new ast::ListLiteralExpression<> (move(items)));
  }

  if (current->type == TOKEN_TYPE_TRUE) {
    next();
    return unique_ptr<ast::Expression<> >(new ast::BoolLiteral<> (true));
  }

  if (current->type == TOKEN_TYPE_FALSE) {
    next();
    return unique_ptr<ast::Expression<> >(new ast::BoolLiteral<> (false));
  }

  if (current->type == TOKEN_TYPE_BANG) {
    nextNoEol();
    return unique_ptr<ast::Expression<> >(new ast::NotExpression<> (parseExpression()));
  }
  
  if (current->type == TOKEN_TYPE_END
      || current->type == TOKEN_TYPE_IF
      || current->type == TOKEN_TYPE_FOR
      || current->type == TOKEN_TYPE_CLOSE_PAREN
      || current->type == TOKEN_TYPE_CLOSE_BRACKET
      || current->type == TOKEN_TYPE_ELSE) {
    ignoreNextNext = true;
  }

  throw ParseError("tried to parse a value, but got an unknown token", errorLocation);
}

void Parser::next() {
  if (ignoreNextNext) {
    ignoreNextNext = false;
    return;
  }

  if (newlineReplacement == TOKEN_TYPE_ERROR) {
    current = tokenizer.next();
  } else {
    current = tokenizer.nextOr(newlineReplacement);
  }
}

void Parser::nextNoEol() {
  if (ignoreNextNext) {
    ignoreNextNext = false;
    return;
  }
  current = tokenizer.nextNoEol();
}

void Parser::nextOr(TokenType newlineType) {
  if (ignoreNextNext) {
    ignoreNextNext = false;
    return;
  }
  current = tokenizer.nextOr(newlineType);
}

