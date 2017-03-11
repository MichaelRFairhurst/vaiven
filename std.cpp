#include "std.h"

#include <iostream>

using namespace std;
using namespace vaiven;

void vaiven::init_std(Functions& funcs) {
  funcs.addNative("print", 1, (void*) print, false);
}

Value vaiven::print(Value value) {
  if (value.isInt()) {
    cout << "Int: " << value.getInt() << endl << endl;
  } else if (value.isVoid()) {
    cout << "void" << endl << endl;
  } else if (value.isTrue()) {
    cout << "true" << endl << endl;
  } else if (value.isFalse()) {
    cout << "false" << endl << endl;
  } else if (value.isPtr()) {
    cout << "Ptr: " << value.getPtr() << endl << endl;
  } else if (value.isDouble()) {
    cout << "Dbl: " << value.getDouble() << endl << endl;
  } else {
    cout << "error: " << value.getRaw();
  }

  return Value();
}
