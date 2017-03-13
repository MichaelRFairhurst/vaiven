#include "std.h"

#include <iostream>

#include "heap.h"

using namespace std;
using namespace vaiven;

void vaiven::init_std(Functions& funcs) {
  funcs.addNative("print", 1, (void*) print, false);
  funcs.addNative("listWith", 1, (void*) listWith, false);
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
    cout << "Ptr: " << value.getPtr();
    if (value.getPtr()->getType() == GCABLE_TYPE_LIST) {
      cout << " (a list)";
    }
    cout << endl << endl;
  } else if (value.isDouble()) {
    cout << "Dbl: " << value.getDouble() << endl << endl;
  } else {
    cout << "error: " << value.getRaw();
  }

  return Value();
}

Value vaiven::listWith(Value value) {
  GcableList* list = globalHeap->newList();
  list->list.push_back(value);
  return Value(list);
}
