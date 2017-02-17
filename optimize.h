#ifndef VAIVEN_HEADER_OPTIMIZE
#define VAIVEN_HEADER_OPTIMIZE

#include "functions.h"
#include "type_info.h"
#include "ast/funcdecl.h"

namespace vaiven {

OverkillFunc optimize(vaiven::Functions& funcs, ast::FuncDecl<vaiven::TypedLocationInfo>& funcDecl);

}

#endif
