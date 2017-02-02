#include "stdio.h"
#include "parser.h"
#include "visitor/print_visitor.h"
#include "visitor/interpreter.h"
#include "visitor/compiler.h"
#include "visitor/autocompiler.h"
#include "visitor/location_resolver.h"

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
  unique_ptr<ast::Node<bool> > cur = parser.nextEvaluatableBlock();

  while (cur.get() != NULL) {
    cout << endl;
    visitor::LocationResolver locResolver;
    cur->accept(locResolver);
    unique_ptr<ast::Expression<Location> > resolved(locResolver.copyStack.top());

    CodeHolder codeHolder;
    codeHolder.init(jt.getCodeInfo());
    codeHolder.setLogger(&logger);
    X86Assembler assembler(&codeHolder);
    X86Compiler cc(&codeHolder);
    //visitor::Compiler compiler(assembler);
    visitor::AutoCompiler compiler(cc);
    compiler.compile(*resolved);
    int (*func)(int rdi, int rsi, int rdx, int rcx, int r8, int r9);
    jt.add(&func, &codeHolder);

    visitor::PrintVisitor printer;
    cur->accept(printer);
    cout << "=";
    visitor::Interpreter interpreter;
    cout << func(1, 2, 3, 4, 5, 6) << endl << endl;
    //cur->accept(interpreter);
    //cout << interpreter.stack.top() << endl;
    //interpreter.stack.pop();

    for (size_t i = 0; i < 5000000000; ++i) {
      func(i, i + 1, i + 2, i + 3, i + 4, i + 5);
      //cur->accept(interpreter);
      //interpreter.stack.pop();
    }

    jt.release(func);
    
    
    cur = parser.nextEvaluatableBlock();
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
      case TOKEN_TYPE_OPEN_PAREN:
        cout << "(" << endl; break;
      case TOKEN_TYPE_CLOSE_PAREN:
        cout << ")" << endl; break;
      case TOKEN_TYPE_MULTIPLY:
        cout << "*" << endl; break;
      case TOKEN_TYPE_DIVIDE:
        cout << "/" << endl; break;
      case TOKEN_TYPE_SEMICOLON:
        cout << ";" << endl; break;
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
