#include "runtime_error.h"

#include <iostream>

using namespace std;

jmp_buf vaiven::errorJmpBuf;

void vaiven::expectedInt() {
  throwError(EXPECTED_INT);
}

void vaiven::expectedBool() {
  throwError(EXPECTED_BOOL);
}

void vaiven::noSuchFunction() {
  throwError(NO_SUCH_FUNCTION);
}

void vaiven::duplicateVar() {
  throwError(DUPLICATE_VAR);
}

void vaiven::noSuchVar() {
  throwError(NO_SUCH_VAR);
}

void vaiven::throwError(ErrorCode code) {
  longjmp(errorJmpBuf, code);
}

void vaiven::defaultHandle(ErrorCode code) {
  switch(code) {
    case EXPECTED_INT:
      cout << "Got a non-integer in an integer operation" << endl;
      break;

    case EXPECTED_BOOL:
      cout << "Got a non-boolean in a boolean operation" << endl;
      break;

    case NO_SUCH_FUNCTION:
      cout << "Called a function that does not exist" << endl;
      break;

    case DUPLICATE_VAR:
      cout << "Duplicate variable" << endl;
      break;

    case NO_SUCH_VAR:
      cout << "Accessed a var that does not exist" << endl;
      break;
  }
}
