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
  EXPECTED_DOUBLE = 3,
  EXPECTED_STR = 4,
  EXPECTED_LIST = 5,
  EXPECTED_OBJ = 6,
  EXPECTED_INT_OR_DOUBLE = 7,
  EXPECTED_STR_OR_INT = 8,
  EXPECTED_STR_OR_INT_OR_DOUBLE = 9,
  EXPECTED_LIST_OR_STR = 10,
  EXPECTED_LIST_OR_OBJ = 11,
  NO_SUCH_FUNCTION = 12,
  DUPLICATE_VAR = 13,
  NO_SUCH_VAR = 14,
  USER_STR = 15,
};

void expectedInt();
void expectedBool();
void expectedDouble();
void expectedStr();
void expectedList();
void expectedObj();
void expectedIntOrDouble();
void expectedStrOrInt();
void expectedStrOrIntOrDouble();
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
