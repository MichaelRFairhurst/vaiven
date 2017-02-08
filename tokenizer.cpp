#include "tokenizer.h"
#include <vector>
using std::vector;

using namespace vaiven;

// quick profiling revealed that a prereserved vector is faster than
// a deque, char[] on the stack, or linked list. 32 should nearly
// eliminate resizes, without using unexpectedly high mem.
#define ID_BUFFER_RESERVE_SIZE 32

bool isNumChar(char c) {
  return c >= '0' && c <= '9';
}

bool isIdStartChar(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isIdChar(char c) {
  return isIdStartChar(c) || isNumChar(c);
}

unique_ptr<Token> Tokenizer::tokenizeEnd() {
  char c = input.peek();
  if (c != 'n') {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('e');
    return tokenizeId(buffer);
  }
  input.get();
  c = input.peek();
  if (c != 'd') {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('e');
    buffer.push_back('n');
    return tokenizeId(buffer);
  }
  input.get();
  if (isIdChar(input.peek())) {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('e');
    buffer.push_back('n');
    buffer.push_back('d');
    return tokenizeId(buffer);
  }

  return unique_ptr<Token>(new Token(TOKEN_TYPE_END));
}

unique_ptr<Token> Tokenizer::tokenizeOf() {
  char c = input.peek();
  if (c != 'f') {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('o');
    return tokenizeId(buffer);
  }
  input.get();
  if (isIdChar(input.peek())) {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('o');
    buffer.push_back('f');
    return tokenizeId(buffer);
  }

  return unique_ptr<Token>(new Token(TOKEN_TYPE_OF));
}

unique_ptr<Token> Tokenizer::tokenizeFn() {
  char c = input.peek();
  if (c != 'n') {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('f');
    return tokenizeId(buffer);
  }
  input.get();
  if (isIdChar(input.peek())) {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('f');
    buffer.push_back('n');
    return tokenizeId(buffer);
  }

  return unique_ptr<Token>(new Token(TOKEN_TYPE_FN));
}

unique_ptr<Token> Tokenizer::tokenizeIs() {
  char c = input.peek();
  if (c != 's') {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('i');
    return tokenizeId(buffer);
  }
  input.get();
  if (isIdChar(input.peek())) {
    vector<char> buffer;
    buffer.reserve(ID_BUFFER_RESERVE_SIZE);
    buffer.push_back('i');
    buffer.push_back('s');
    return tokenizeId(buffer);
  }

  return unique_ptr<Token>(new Token(TOKEN_TYPE_IS));
}

unique_ptr<Token> Tokenizer::tokenizeId(vector<char>& buffer) {
  char c = input.peek();
  while (isIdChar(c)) {
    buffer.push_back(c);
  
    input.get();
    c = input.peek();
  }
  return unique_ptr<Token>(new StringToken(TOKEN_TYPE_ID, string(buffer.begin(), buffer.end())));
}

unique_ptr<Token> Tokenizer::next() {
  char c = input.get();
  switch(c) {
    case '+':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_PLUS));
    case '-':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_MINUS));
    case '(':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_OPEN_PAREN));
    case ')':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_CLOSE_PAREN));
    case '{':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_OPEN_BRACE));
    case '}':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_CLOSE_BRACE));
    case '*':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_MULTIPLY));
    case '/':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_DIVIDE));
    case ';':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_SEMICOLON));
    case ',':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_COMMA));
    case EOF:
      return unique_ptr<Token>(new Token(TOKEN_TYPE_EOF));
    case 'i':
      return tokenizeIs();
    case 'e':
      return tokenizeEnd();
    case 'f':
      return tokenizeFn();
    case 'o':
      return tokenizeOf();
    case ' ':
    case '\n':
      return next();

    default:
      if (isNumChar(c)) {
        int val = c - '0';
	char next = input.peek();
	while (isNumChar(next)) {
	  val = val * 10 + (next - '0');
          input.get();
          next = input.peek();
	}
	if (next == '.') {
	  // todo doubles
        }

        return unique_ptr<Token>(new IntegerToken(TOKEN_TYPE_INTEGER, val));
      }
      if (isIdStartChar(c)) {
	vector<char> buffer;
	buffer.reserve(ID_BUFFER_RESERVE_SIZE);
	buffer.push_back(c);

        return tokenizeId(buffer);
      }
  }

  return unique_ptr<Token>(new StringToken(TOKEN_TYPE_ERROR, string(1, c)));
}
