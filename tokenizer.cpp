#include "tokenizer.h"
#include <vector>
using std::vector;

using namespace vaiven;

bool isNumChar(char c) {
  return c >= '0' && c <= '9';
}

bool isIdStartChar(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isIdChar(char c) {
  return isIdStartChar(c) || isNumChar(c);
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
    case '*':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_MULTIPLY));
    case '/':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_DIVIDE));
    case ';':
      return unique_ptr<Token>(new Token(TOKEN_TYPE_SEMICOLON));
    case EOF:
      return unique_ptr<Token>(new Token(TOKEN_TYPE_EOF));
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
	int len = 1;
	char next = input.peek();
	// quick profiling revealed that a prereserved vector is faster than
	// a deque, char[] on the stack, or linked list. 32 should nearly
	// eliminate resizes, without using unexpectedly high mem.
	vector<char> buffer;
	buffer.reserve(32);
	buffer.push_back(c);

	while (isIdChar(next)) {
	  buffer.push_back(next);

          input.get();
          next = input.peek();
	}
        return unique_ptr<Token>(new StringToken(TOKEN_TYPE_ID, string(buffer.begin(), buffer.end())));
      }
  }

  return unique_ptr<Token>(new StringToken(TOKEN_TYPE_ERROR, string(1, c)));
}
