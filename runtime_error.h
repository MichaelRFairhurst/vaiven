#ifndef VAIVEN_VISITOR_HEADER_RUNTIME_ERROR
#define VAIVEN_VISITOR_HEADER_RUNTIME_ERROR

#include <csetjmp>

namespace vaiven {

extern jmp_buf errorJmpBuf;

enum ErrorCode {
  EXPECTED_INT = 1,
  EXPECTED_BOOL = 2,
  NO_SUCH_FUNCTION = 3,
  DUPLICATE_VAR = 4,
  NO_SUCH_VAR = 5,
};

void expectedInt();
void expectedBool();
void noSuchFunction();
void duplicateVar();
void noSuchVar();

void throwError(ErrorCode code);

void defaultHandle(ErrorCode code);

}

#endif
