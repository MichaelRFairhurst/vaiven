#include "runtime_error.h"

#include <iostream>

using namespace std;

jmp_buf vaiven::errorJmpBuf;
std::string vaiven::errMsg;

void vaiven::expectedInt() {
  throwError(EXPECTED_INT);
}

void vaiven::expectedBool() {
  throwError(EXPECTED_BOOL);
}

void vaiven::expectedDouble() {
  throwError(EXPECTED_DOUBLE);
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

void vaiven::expectedStr() {
  throwError(EXPECTED_STR);
}

void vaiven::expectedList() {
  throwError(EXPECTED_LIST);
}

void vaiven::expectedObj() {
  throwError(EXPECTED_OBJ);
}

void vaiven::expectedIntOrDouble() {
  throwError(EXPECTED_INT_OR_DOUBLE);
}

void vaiven::expectedStrOrInt() {
  throwError(EXPECTED_STR_OR_INT);
}

void vaiven::expectedStrOrIntOrDouble() {
  throwError(EXPECTED_STR_OR_INT_OR_DOUBLE);
}

void vaiven::expectedListOrStr() {
  throwError(EXPECTED_LIST_OR_STR);
}

void vaiven::expectedListOrObj() {
  throwError(EXPECTED_LIST_OR_OBJ);
}

void vaiven::errString(std::string msg) {
  vaiven::errMsg = msg;
  throwError(USER_STR);
}

void vaiven::throwError(ErrorCode code) {
  longjmp(errorJmpBuf, code);
}

void vaiven::defaultHandle(ErrorCode code) {
  cout << "Error: ";
  switch(code) {
    case EXPECTED_INT:
      cout << "Got a non-integer in an integer operation" << endl;
      break;

    case EXPECTED_BOOL:
      cout << "Got a non-boolean in a boolean operation" << endl;
      break;

    case EXPECTED_DOUBLE:
      cout << "Got a non-double in a double operation" << endl;
      break;

    case EXPECTED_STR:
      cout << "Got a non-string in a string operation" << endl;
      break;

    case EXPECTED_LIST:
      cout << "Got a non-list in a list operation" << endl;
      break;

    case EXPECTED_OBJ:
      cout << "Got a non-object in an object operation" << endl;
      break;

    case EXPECTED_INT_OR_DOUBLE:
      cout << "Got a wrong type in an operation for ints/doubles" << endl;
      break;

    case EXPECTED_STR_OR_INT:
      cout << "Got a wrong type in an operation for ints/strings" << endl;
      break;

    case EXPECTED_STR_OR_INT_OR_DOUBLE:
      cout << "Got a wrong type in an operation for ints/strings/doubles" << endl;
      break;

    case EXPECTED_LIST_OR_STR:
      cout << "Got a wrong type in an operation for lists/strings" << endl;
      break;

    case EXPECTED_LIST_OR_OBJ:
      cout << "Got a wrong type in an operation for objects/lists" << endl;
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

    case USER_STR:
      cout << vaiven::errMsg << endl;
      break;
  }
}
