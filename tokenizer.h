#ifndef VAIVEN_HEADER_TOKENIZER
#define VAIVEN_HEADER_TOKENIZER

#include <iostream>
#include <string>
#include <memory>

using std::istream;
using std::string;
using std::unique_ptr;

namespace vaiven {

class Token;

class Tokenizer {
  public:
    Tokenizer(istream& stream) : input(stream) {}

    unique_ptr<Token> next();
  private:
    istream& input;
  
};

enum TokenType {
  TOKEN_TYPE_ID,
  TOKEN_TYPE_INTEGER,
  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
  TOKEN_TYPE_DIVIDE,
  TOKEN_TYPE_MULTIPLY,
  TOKEN_TYPE_OPEN_PAREN,
  TOKEN_TYPE_CLOSE_PAREN,
  TOKEN_TYPE_SEMICOLON,
  TOKEN_TYPE_ERROR,
  TOKEN_TYPE_EOF
};

class Token {
  public:
  Token(TokenType type) : type(type) {}
  TokenType type;
  virtual ~Token() {};
};

class StringToken : public Token {
  public:
  StringToken(TokenType type, string lexeme) : lexeme(lexeme), Token(type) {}
  TokenType type;
  string lexeme;
  virtual ~StringToken() {};
};

class IntegerToken : public Token {
  public:
  IntegerToken(TokenType type, int value) : value(value), Token(type) {}
  TokenType type;
  int value;
  virtual ~IntegerToken() {};
};

}

#endif
