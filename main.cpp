#include "stdio.h"
#include "parser.h"
#include "visitor/print_visitor.h"
#include "visitor/interpreter.h"
#include "visitor/compiler.h"
#include "visitor/autocompiler.h"
#include "visitor/location_resolver.h"

#include <stdint.h>

#include "asmjit/src/asmjit/asmjit.h"

using std::cin;
using std::cout;
using std::endl;

using namespace vaiven;
using namespace asmjit;

void printTokenStream(Tokenizer& tokenizer);
void printExpressionStream(Parser& tokenizer);

int main() {
  Tokenizer tokenizer(cin);
  Parser parser (tokenizer);

  //printTokenStream(tokenizer);
  printExpressionStream(parser);
}

void printExpressionStream(Parser& parser) {
  JitRuntime jt;
  FileLogger logger(stdout);
  unique_ptr<ast::Node<bool> > cur = parser.parseLogicalGroup();

  while (cur.get() != NULL) {
    visitor::PrintVisitor printer;
    cur->accept(printer);
    cout << endl;

    if (!parser.lastLogicalGroupWasEvaluatable) {
      visitor::LocationResolver locResolver;
      cur->accept(locResolver);
      unique_ptr<ast::Node<Location> > resolved(locResolver.nodeCopyStack.top());

      CodeHolder codeHolder;
      codeHolder.init(jt.getCodeInfo());
      codeHolder.setLogger(&logger);
      X86Assembler assembler(&codeHolder);
      X86Compiler cc(&codeHolder);
      //visitor::Compiler compiler(assembler);
      visitor::AutoCompiler compiler(cc);
      compiler.compile(*resolved, locResolver.argIndexes.size());
      int64_t (*func)(int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t r9, int64_t stack1, int64_t stack2);
      jt.add(&func, &codeHolder);
      int64_t result = func(1, 2, 3, 4, 5, 6, 7, 8);
      cout << result << endl << endl;
      jt.release(func);
      cur = parser.parseLogicalGroup();
      continue;
    }

    cout << "=";
    visitor::Interpreter interpreter;
    //std::vector<int> args;
    //args.push_back(1); args.push_back(2); args.push_back(3);
    //args.push_back(4); args.push_back(5); args.push_back(6);
    //int result = interpreter.interpret(*cur /*, args, &locResolver.argIndexes*/);
    int result = interpreter.interpret(*cur);
    cout << result << endl << endl;

    for (size_t i = 0; i < /*5*/000000/*000*/; ++i) {
      //func(i, i + 1, i + 2, i + 3, i + 4, i + 5, i + 6, i + 7);
      //interpreter.interpret(*cur, args, &locResolver.argIndexes);
      interpreter.interpret(*cur);
    }
    
    cur = parser.parseLogicalGroup();
  }
}

void printTokenStream(Tokenizer& tokenizer) {
  unique_ptr<Token> cur;
  do {
    cur = tokenizer.next();
    switch(cur->type) {
      case TOKEN_TYPE_PLUS:
        cout << "+" << endl; break;
      case TOKEN_TYPE_MINUS:
        cout << "-" << endl; break;
      case TOKEN_TYPE_OPEN_BRACE:
        cout << "{" << endl; break;
      case TOKEN_TYPE_CLOSE_BRACE:
        cout << "}" << endl; break;
      case TOKEN_TYPE_OPEN_PAREN:
        cout << "(" << endl; break;
      case TOKEN_TYPE_CLOSE_PAREN:
        cout << ")" << endl; break;
      case TOKEN_TYPE_MULTIPLY:
        cout << "*" << endl; break;
      case TOKEN_TYPE_DIVIDE:
        cout << "/" << endl; break;
      case TOKEN_TYPE_COMMA:
        cout << "," << endl; break;
      case TOKEN_TYPE_SEMICOLON:
        cout << ";" << endl; break;
      case TOKEN_TYPE_FN:
        cout << "fn" << endl; break;
      case TOKEN_TYPE_END:
        cout << "end" << endl; break;
      case TOKEN_TYPE_IS:
        cout << "is" << endl; break;
      case TOKEN_TYPE_OF:
        cout << "of" << endl; break;
      case TOKEN_TYPE_INTEGER:
	{
	  IntegerToken* inttok = static_cast<IntegerToken*>(cur.get());
          cout << "int" << inttok->value << endl;
	}
	break;
      case TOKEN_TYPE_ID:
	{
	  StringToken* strtok = static_cast<StringToken*>(cur.get());
          cout << "id" << strtok->lexeme << endl;
	}
	break;
      case TOKEN_TYPE_ERROR:
	{
	  StringToken* strtok = static_cast<StringToken*>(cur.get());
          cout << "error: " << strtok->lexeme << endl;
	}
	break;
      case TOKEN_TYPE_EOF:
        cout << "eof" << endl; break;
    }
  } while(cur->type != TOKEN_TYPE_EOF);
}
