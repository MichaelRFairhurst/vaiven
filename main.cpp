#include "stdio.h"
#include "parser.h"
#include "functions.h"
#include "type_info.h"
#include "runtime_error.h"
#include "std.h"
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
  Parser parser(tokenizer);

  //printTokenStream(tokenizer);
  printExpressionStream(parser);
}


// Error handler that just prints the error and lets AsmJit ignore it.
class PrintErrorHandler : public asmjit::ErrorHandler {
public:
  // Return `true` to set last error to `err`, return `false` to do nothing.
  bool handleError(asmjit::Error err, const char* message, asmjit::CodeEmitter* origin) override {
    fprintf(stderr, "Error: %s\n", message);
    return false;
  }
};

void printExpressionStream(Parser& parser) {
  Functions funcs;
  init_std(funcs);
  FileLogger logger(stdout);
  PrintErrorHandler eh;
  unique_ptr<ast::Node<> > cur = parser.parseLogicalGroup();
  visitor::Interpreter interpreter(funcs);

  while (cur.get() != NULL || parser.errors.size() > 0) {
    if (parser.errors.size() > 0) {
      for (vector<ParseError>::iterator it = parser.errors.begin(); it != parser.errors.end(); ++it) {
        cout << "Parse Error: " << it->error << " at " << it->location << endl;
      }
      parser.errors.clear();

      if (cur.get() == NULL) {
        cur = parser.parseLogicalGroup();
        continue;
      }
    }

    if (!parser.lastLogicalGroupWasEvaluatable) {
      visitor::LocationResolver locResolver;
      cur->accept(locResolver);
      unique_ptr<ast::Node<TypedLocationInfo> > resolved(locResolver.nodeCopyStack.top());

      CodeHolder codeHolder;
      codeHolder.init(funcs.runtime.getCodeInfo());
#ifdef DISASSEMBLY_DIAGNOSTICS
      codeHolder.setErrorHandler(&eh);
      codeHolder.setLogger(&logger);
#endif
      X86Assembler assembler(&codeHolder);
      X86Compiler cc(&codeHolder);
      //visitor::Compiler compiler(assembler, codeHolder, funcs);
      try {
        visitor::AutoCompiler compiler(cc, codeHolder, funcs);
        compiler.compile(*resolved);
        resolved.release(); // compiler owns pointer now
      } catch (DuplicateFunctionError e) {
        cout << "function " << e.name << " already defined" << endl;
      }
      cur = parser.parseLogicalGroup();
      continue;
    }


    int error = setjmp(errorJmpBuf);
    if (error) {
      defaultHandle((vaiven::ErrorCode) error);
      cur = parser.parseLogicalGroup();
      continue;
    }

    Value result;
    try {
      result = interpreter.interpret(*cur);
    } catch(Value v) {
      result = v;
    }
    print(result);

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
          cout << "Error: " << strtok->lexeme << endl;
	}
	break;
      case TOKEN_TYPE_EOF:
        cout << "eof" << endl; break;
    }
  } while(cur->type != TOKEN_TYPE_EOF);
}
