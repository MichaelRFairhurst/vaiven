#include "stdio.h"
#include "parser.h"
#include "functions.h"
#include "type_info.h"
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


// Error handler that just prints the error and lets AsmJit ignore it.
class PrintErrorHandler : public asmjit::ErrorHandler {
public:
  // Return `true` to set last error to `err`, return `false` to do nothing.
  bool handleError(asmjit::Error err, const char* message, asmjit::CodeEmitter* origin) override {
    fprintf(stderr, "ERROR: %s\n", message);
    return false;
  }
};

void printExpressionStream(Parser& parser) {
  Functions funcs;
  FileLogger logger(stdout);
  PrintErrorHandler eh;
  unique_ptr<ast::Node<> > cur = parser.parseLogicalGroup();

  while (cur.get() != NULL) {
    //visitor::PrintVisitor printer;
    //cur->accept(printer);
    //cout << endl;

    if (!parser.lastLogicalGroupWasEvaluatable) {
      visitor::LocationResolver locResolver;
      cur->accept(locResolver);
      unique_ptr<ast::Node<TypedLocationInfo> > resolved(locResolver.nodeCopyStack.top());

      CodeHolder codeHolder;
      codeHolder.init(funcs.runtime.getCodeInfo());
      codeHolder.setErrorHandler(&eh);
      codeHolder.setLogger(&logger);
      X86Assembler assembler(&codeHolder);
      X86Compiler cc(&codeHolder);
      //visitor::Compiler compiler(assembler);
      visitor::AutoCompiler compiler(cc, codeHolder, funcs);
      compiler.compile(*resolved);
      resolved.release(); // compiler owns pointer now
      //int64_t result = func(1, 2, 3, 4, 5, 6, 7, 8);
      //cout << result << endl << endl;
      cur = parser.parseLogicalGroup();
      continue;
    }

    cout << "=";
    visitor::Interpreter interpreter(funcs);
    //std::vector<int> args;
    //args.push_back(1); args.push_back(2); args.push_back(3);
    //args.push_back(4); args.push_back(5); args.push_back(6);
    //int result = interpreter.interpret(*cur /*, args, &locResolver.argIndexes*/);
    Value result = interpreter.interpret(*cur);
    if (result.isInt()) {
      cout << "Int: " << result.getInt() << endl << endl;
    } else if (result.isVoid()) {
      cout << "void" << endl << endl;
    } else if (result.isTrue()) {
      cout << "true" << endl << endl;
    } else if (result.isFalse()) {
      cout << "false" << endl << endl;
    } else if (result.isPtr()) {
      cout << "Ptr: " << result.getPtr() << endl << endl;
    } else if (result.isDouble()) {
      cout << "Dbl: " << result.getDouble() << endl << endl;
    } else {
      cout << "error" << result.getRaw();
    }

    for (size_t i = 0; i < /*5*/000000/*000*/; ++i) {
      //func(i, i + 1, i + 2, i + 3, i + 4, i + 5, i + 6, i + 7);
      //interpreter.interpret(*cur, args, &locResolver.argIndexes);
      //interpreter.interpret(*cur);
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
      case TOKEN_TYPE_EQ:
        cout << "=" << endl; break;
      case TOKEN_TYPE_EQEQ:
        cout << "==" << endl; break;
      case TOKEN_TYPE_GT:
        cout << ">" << endl; break;
      case TOKEN_TYPE_COMMA:
        cout << "," << endl; break;
      case TOKEN_TYPE_SEMICOLON:
        cout << ";" << endl; break;
      case TOKEN_TYPE_FN:
        cout << "fn" << endl; break;
      case TOKEN_TYPE_END:
        cout << "end" << endl; break;
      case TOKEN_TYPE_ELSE:
        cout << "else" << endl; break;
      case TOKEN_TYPE_VAR:
        cout << "var" << endl; break;
      case TOKEN_TYPE_IS:
        cout << "is" << endl; break;
      case TOKEN_TYPE_IF:
        cout << "if" << endl; break;
      case TOKEN_TYPE_OF:
        cout << "of" << endl; break;
      case TOKEN_TYPE_DO:
        cout << "do" << endl; break;
      case TOKEN_TYPE_RET:
        cout << "ret" << endl; break;
      case TOKEN_TYPE_TRUE:
        cout << "true" << endl; break;
      case TOKEN_TYPE_FALSE:
        cout << "false" << endl; break;
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
