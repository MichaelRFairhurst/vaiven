#include "recompiler.h"

#include "../ast/all.h"
#include "../value.h"
#include "../runtime_error.h"

#include <iostream>
#include <stdint.h>

using namespace asmjit;
using namespace vaiven::visitor;

void ReCompiler::visitFuncDecl(FuncDecl<TypedLocationInfo>& decl) {
  uint8_t sigArgs[decl.args.size()];

  for (int i = 0; i < decl.args.size(); ++i) {
    sigArgs[i] = TypeIdOf<int64_t>::kTypeId;
  }

  FuncSignature sig;
  sig.init(CallConv::kIdHost, TypeIdOf<int64_t>::kTypeId, sigArgs, decl.args.size());
  curFunc = cc.addFunc(sig);
  curFuncName = decl.name;

  Label deoptimizeLabel = cc.newLabel();
  X86Gp checkArg = cc.newInt64();
  for (int i = 0; i < decl.args.size(); ++i) {
    X86Gp arg = cc.newInt64();
    cc.setArg(i, arg);
    argRegs.push_back(arg);

    if (usageInfo.argShapes[i].isPureInt()) {
      cc.mov(checkArg, arg);
      cc.shr(checkArg, INT_TAG_SHIFT);
      cc.cmp(checkArg, INT_TAG_SHIFTED);
      cc.jne(deoptimizeLabel);
    }
  }

  typeErrorLabel = cc.newLabel();

  TypedLocationInfo endType;
  for(vector<unique_ptr<Statement<TypedLocationInfo> > >::iterator it = decl.statements.begin();
      it != decl.statements.end();
      ++it) {
    (*it)->accept(*this);
    endType = (*it)->resolvedData;
  }

  if (vRegs.size()) {
    box(vRegs.top(), endType);
    cc.ret(vRegs.top());
  } else {
    X86Gp voidReg = cc.newInt64();
    cc.mov(voidReg, Value().getRaw());
    cc.ret(voidReg);
  }
  
  cc.bind(deoptimizeLabel);
  generateTypeErrorProlog();
  cc.endFunc();
  cc.finalize();


  OverkillFunc fptr;
  funcs.runtime.add(&fptr, &codeHolder);
  funcs.funcs[decl.name]->slowfptr = funcs.funcs[decl.name]->fptr;
  funcs.funcs[decl.name]->fptr = fptr;
}
