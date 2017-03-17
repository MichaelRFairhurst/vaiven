#include "std.h"

#include <iostream>

#include "heap.h"
#include "runtime_error.h"

using namespace std;
using namespace vaiven;

void vaiven::init_std(Functions& funcs) {
  funcs.addNative("print", 1, (void*) print, false);
  funcs.addNative("list", 0, (void*) newList, true);
  funcs.addNative("append", 2, (void*) append, false);
  funcs.addNative("len", 1, (void*) len, false); // not pure because can throw
  funcs.addNative("assert", 1, (void*) assert, false);
  funcs.addNative("object", 0, (void*) object, true);
  funcs.addNative("keys", 1, (void*) keys, false); // not pure because can throw
  funcs.addNative("set", 3, (void*) set, false);
  funcs.addNative("get", 2, (void*) get, false); // not pure because can throw
  funcs.addNative("cmp", 2, (void*) cmp, true);
  funcs.addNative("toString", 1, (void*) toString, true);
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
    if (value.getPtr()->getType() == GCABLE_TYPE_LIST) {
      GcableList* list = (GcableList*) value.getPtr();
      cout << "[";
      for (vector<Value>::iterator it = list->list.begin(); it != list->list.end(); ++it) {
        print(*it);
        cout << ",";
      }
      cout << "]";
    } else if (value.getPtr()->getType() == GCABLE_TYPE_STRING) {
      cout << ((GcableString*) value.getPtr())->str;
    }
    cout << endl << endl;
  } else if (value.isDouble()) {
    cout << "Dbl: " << value.getDouble() << endl << endl;
  } else {
    cout << "Error: unexpected value " << value.getRaw();
  }

  return Value();
}

Value vaiven::newList() {
  GcableList* list = globalHeap->newList();
  return Value(list);
}

Value vaiven::append(Value lhs, Value rhs) {
  if (!lhs.isPtr()) {
    expectedInt();
  }

  if (lhs.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) lhs.getPtr();
    list->list.push_back(rhs);
  } else if (rhs.isPtr() && lhs.getPtr()->getType() == GCABLE_TYPE_STRING) {
    if (rhs.getPtr()->getType() == GCABLE_TYPE_STRING) {
      GcableString* strleft = (GcableString*) lhs.getPtr();
      GcableString* strright = (GcableString*) rhs.getPtr();
      GcableString* result = globalHeap->newString();
      result->str = strleft->str + strright->str;
      return Value(result);
    } else {
      expectedInt();
    }
  } else {
    expectedInt();
  }

  return Value();
}

Value vaiven::len(Value subject) {
  if (!subject.isPtr()) {
    expectedInt();
  }

  if (subject.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) subject.getPtr();
    return Value((int) list->list.size());
  } else if (subject.getPtr()->getType() == GCABLE_TYPE_STRING) {
    GcableString* str = (GcableString*) subject.getPtr();
    return Value((int) str->str.size());
  } else {
    expectedInt();
  }
}

Value vaiven::assert(Value expectation) {
  if (!expectation.isBool()) {
    expectedBool();
  }

  if (!expectation.getBool()) {
    expectedBool();
  }

  return Value();
}

Value vaiven::object() {
  return Value(globalHeap->newObject());
}

Value vaiven::keys(Value subject) {
  if (!subject.isPtr()) {
    expectedInt();
  }

  if (subject.getPtr()->getType() != GCABLE_TYPE_OBJECT) {
    expectedInt();
  }

  GcableObject* object = (GcableObject*) subject.getPtr();
  GcableList* list = globalHeap->newList();

  for (unordered_map<string, Value>::iterator it = object->properties.begin();
      it != object->properties.end();
      ++it) {
    GcableString* str = globalHeap->newString();
    str->str = it->first;
    list->list.push_back(Value(str));
  }

  return Value(list);
}

Value vaiven::set(Value subject, Value propOrIndex, Value value) {
  if (!subject.isPtr()) {
    expectedInt();
  }

  if (subject.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) subject.getPtr();
    if (propOrIndex.isInt()) {
      int index = propOrIndex.getInt();
      while (list->list.size() <= index) {
        list->list.push_back(Value());
      }
      list->list[index] = value;
    } else {
      expectedInt();
    }
  } else if (subject.getPtr()->getType() == GCABLE_TYPE_OBJECT) {
    GcableObject* object = (GcableObject*) subject.getPtr();
    if (propOrIndex.isPtr()
        && propOrIndex.getPtr()->getType() == GCABLE_TYPE_STRING) {
      GcableString* attrname = (GcableString*) propOrIndex.getPtr();
      object->properties[attrname->str] = value;
    } else {
      expectedInt();
    }
  } else {
    expectedInt();
  }

  return Value();
}

Value vaiven::get(Value subject, Value propOrIndex) {
  if (!subject.isPtr()) {
    expectedInt();
  }

  if (subject.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) subject.getPtr();
    if (propOrIndex.isInt()) {
      int index = propOrIndex.getInt();
      if (list->list.size() <= index) {
        return Value();
      }
      return list->list[index];
    } else {
      expectedInt();
    }
  } else if (subject.getPtr()->getType() == GCABLE_TYPE_OBJECT) {
    GcableObject* object = (GcableObject*) subject.getPtr();
    if (propOrIndex.isPtr()
        && propOrIndex.getPtr()->getType() == GCABLE_TYPE_STRING) {
      GcableString* attrname = (GcableString*) propOrIndex.getPtr();
      if (object->properties.find(attrname->str) == object->properties.end()) {
        return Value();
      }
      return object->properties[attrname->str];
    } else {
      expectedInt();
    }
  } else {
    expectedInt();
  }

  return Value();
}

Value vaiven::cmp(Value a, Value b) {
  if (a.isPtr() && b.isPtr()
      && a.getPtr()->getType() == GCABLE_TYPE_STRING
      && b.getPtr()->getType() == GCABLE_TYPE_STRING) {
    return Value(
        ((GcableString*) a.getPtr())->str == ((GcableString*) b.getPtr())->str);
  }
  return Value(a.getRaw() == b.getRaw());
}

Value vaiven::toString(Value subject) {
  if (subject.isPtr() && subject.getPtr()->getType() == GCABLE_TYPE_STRING) {
    return subject;
  }
  
  if (subject.isBool() && subject.getBool()) {
    GcableString* str = globalHeap->newString();
    str->str = "true";
    return Value(str);
  }
  
  if (subject.isBool() && !subject.getBool()) {
    GcableString* str = globalHeap->newString();
    str->str = "false";
    return Value(str);
  }
  
  if (subject.isVoid()) {
    GcableString* str = globalHeap->newString();
    str->str = "void";
    return Value(str);
  }

  return Value(4);
}
