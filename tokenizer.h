#ifndef VAIVEN_HEADER_TOKENIZER
#define VAIVEN_HEADER_TOKENIZER

#include <iostream>
#include <string>
#include <memory>
#include <vector>

using std::istream;
using std::string;
using std::vector;
using std::unique_ptr;

namespace vaiven {

class Token;

enum TokenType {
  TOKEN_TYPE_ID,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_FN,
  TOKEN_TYPE_END,
  TOKEN_TYPE_IS,
  TOKEN_TYPE_IF,
  TOKEN_TYPE_OF,
  TOKEN_TYPE_VAR,
  TOKEN_TYPE_RET,
  TOKEN_TYPE_FOR,
  TOKEN_TYPE_ELSE,
  TOKEN_TYPE_DO,
  TOKEN_TYPE_TRUE,
  TOKEN_TYPE_FALSE,
  TOKEN_TYPE_INTEGER,
  TOKEN_TYPE_DOT,
  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
  TOKEN_TYPE_DIVIDE,
  TOKEN_TYPE_MULTIPLY,
  TOKEN_TYPE_EQ,
  TOKEN_TYPE_EQEQ,
  TOKEN_TYPE_BANGEQ,
  TOKEN_TYPE_PLUSEQ,
  TOKEN_TYPE_SUBEQ,
  TOKEN_TYPE_MULEQ,
  TOKEN_TYPE_DIVEQ,
  TOKEN_TYPE_BANG,
  TOKEN_TYPE_GT,
  TOKEN_TYPE_GTE,
  TOKEN_TYPE_LT,
  TOKEN_TYPE_LTE,
  TOKEN_TYPE_OPEN_PAREN,
  TOKEN_TYPE_CLOSE_PAREN,
  TOKEN_TYPE_OPEN_BRACKET,
  TOKEN_TYPE_CLOSE_BRACKET,
  TOKEN_TYPE_OPEN_BRACE,
  TOKEN_TYPE_CLOSE_BRACE,
  TOKEN_TYPE_COMMA,
  TOKEN_TYPE_SEMICOLON,
  TOKEN_TYPE_ERROR,
  TOKEN_TYPE_EOF,
  TOKEN_TYPE_IGNORABLE_NEWLINE // this is for nextNoEol only
};

class Tokenizer {
  public:
    Tokenizer(istream& stream) : input(stream) {}

    unique_ptr<Token> next();
    unique_ptr<Token> nextNoEol();
    unique_ptr<Token> nextOr(TokenType newlineType);
  private:
    unique_ptr<Token> tokenizeIsOrIf();
    unique_ptr<Token> tokenizeEndOrElse();
    unique_ptr<Token> tokenizeRet();
    unique_ptr<Token> tokenizeDo();
    unique_ptr<Token> tokenizeFnOrFalseOrFor();
    unique_ptr<Token> tokenizeTrue();
    unique_ptr<Token> tokenizeOf();
    unique_ptr<Token> tokenizeVar();
    unique_ptr<Token> tokenizeId(vector<char>& buffer);
    istream& input;
  
};

class Token {
  public:
  Token(TokenType type) : type(type) {}
  TokenType type;
  virtual ~Token() {};
  virtual Token* copy();
};

class StringToken : public Token {
  public:
  StringToken(TokenType type, string lexeme) : lexeme(lexeme), Token(type) {}
  TokenType type;
  string lexeme;
  virtual ~StringToken() {};
  StringToken* copy();
};

class IntegerToken : public Token {
  public:
  IntegerToken(TokenType type, int value) : value(value), Token(type) {}
  TokenType type;
  int value;
  virtual ~IntegerToken() {};
  IntegerToken* copy();
};

}

#endif
