#ifndef VAIVEN_HEADER_FIRST_COMPILE
#define VAIVEN_HEADER_FIRST_COMPILE

#include "functions.h"
#include "type_info.h"
#include "error_compiler.h"
#include "ast/funcdecl.h"
#include "asmjit/src/asmjit/asmjit.h"

namespace vaiven {

void firstCompile(vaiven::Functions& funcs, ast::FuncDecl<>& funcDecl);

void firstOptimizations(ast::FuncDecl<>& decl, Functions& funcs, FunctionUsage& usageInfo, asmjit::X86Compiler& cc, vector<asmjit::X86Gp>& argRegs, ErrorCompiler& error, asmjit::Label funcLabel);
void generateOptimizeEpilog(ast::FuncDecl<>& decl, asmjit::FuncSignature& sig, asmjit::X86Compiler& cc, asmjit::Label& optimizeLabel, vector<asmjit::X86Gp>& argRegs, Functions& funcs);
void generateTypeShapeProlog(ast::FuncDecl<>& decl, FunctionUsage* usage, asmjit::X86Compiler& cc, asmjit::Label* optimizeLabel, vector<asmjit::X86Gp>& argRegs);

}

#endif
