#ifndef VAIVEN_VISITOR_HEADER_RUNTIME_ERROR
#define VAIVEN_VISITOR_HEADER_RUNTIME_ERROR

#include <csetjmp>
#include <string>

namespace vaiven {

extern jmp_buf errorJmpBuf;
extern std::string errMsg;

enum ErrorCode {
  EXPECTED_INT = 1,
  EXPECTED_BOOL = 2,
  EXPECTED_STR = 3,
  EXPECTED_LIST = 4,
  EXPECTED_OBJ = 5,
  EXPECTED_STR_OR_INT = 6,
  EXPECTED_LIST_OR_STR = 7,
  EXPECTED_LIST_OR_OBJ = 8,
  NO_SUCH_FUNCTION = 9,
  DUPLICATE_VAR = 10,
  NO_SUCH_VAR = 11,
  USER_STR = 12,
};

void expectedInt();
void expectedBool();
void expectedStr();
void expectedList();
void expectedObj();
void expectedStrOrInt();
void expectedListOrStr();
void expectedListOrObj();
void noSuchFunction();
void duplicateVar();
void noSuchVar();
void errString(std::string msg);

void throwError(ErrorCode code);

void defaultHandle(ErrorCode code);

}

#endif
