#include "std.h"
#include <cmath>

#include <iostream>
#include <iomanip>

#include "heap.h"
#include "runtime_error.h"

using namespace std;
using namespace vaiven;

void vaiven::init_std(Functions& funcs) {
  funcs.addNative("print", 1, (void*) print, false);
  funcs.addNative("append", 2, (void*) append, false);
  funcs.addNative("len", 1, (void*) len, false); // not pure because can throw
  funcs.addNative("assert", 1, (void*) assert, false);
  funcs.addNative("object", 0, (void*) object, true);
  funcs.addNative("keys", 1, (void*) keys, false); // not pure because can throw
  funcs.addNative("toString", 1, (void*) toString, true);
  funcs.addNative("sqrt", 1, (void*) sqrt, false); // not pure because can throw
  funcs.addNative("round", 1, (void*) round, false); // not pure because can throw
  funcs.addNative("floor", 1, (void*) floor, false); // not pure because can throw
  funcs.addNative("ceil", 1, (void*) ceil, false); // not pure because can throw

  // for printing doubles
  cout << setprecision(100);
}

Value vaiven::print(Value value) {
  if (value.isInt()) {
    cout << "Int: " << value.getInt() << endl << endl;
  } else if (value.isDouble()) {
    cout << "Double: " << value.getDouble() << endl << endl;
  } else {
    cout << toStringCpp(value) << endl << endl;
  }

  return Value();
}

Value vaiven::append(Value lhs, Value rhs) {
  if (!lhs.isPtr()) {
    expectedListOrStr();
  }

  if (lhs.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) lhs.getPtr();
    list->list.push_back(rhs);
  } else if (lhs.isPtr() && lhs.getPtr()->getType() == GCABLE_TYPE_STRING) {
    if (rhs.isPtr() && rhs.getPtr()->getType() == GCABLE_TYPE_STRING) {
      GcableString* strleft = (GcableString*) lhs.getPtr();
      GcableString* strright = (GcableString*) rhs.getPtr();
      GcableString* result = globalHeap->newString();
      result->str = strleft->str + strright->str;
      return Value(result);
    } else {
      expectedStr();
    }
  } else {
    expectedListOrStr();
  }

  return Value();
}

Value vaiven::len(Value subject) {
  if (!subject.isPtr()) {
    expectedListOrStr();
  }

  if (subject.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) subject.getPtr();
    return Value((int) list->list.size());
  } else if (subject.getPtr()->getType() == GCABLE_TYPE_STRING) {
    GcableString* str = (GcableString*) subject.getPtr();
    return Value((int) str->str.size());
  } else {
    expectedListOrStr();
  }
}

Value vaiven::assert(Value expectation) {
  if (!expectation.isBool()) {
    expectedBool();
  }

  if (!expectation.getBool()) {
    // TODO custom message
    errString("assertion failed");
  }

  return Value();
}

Value vaiven::object() {
  return Value(globalHeap->newObject());
}

Value vaiven::keys(Value subject) {
  if (!subject.isPtr()) {
    expectedObj();
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


Value vaiven::toString(Value subject) {
  if (subject.isPtr() && subject.getPtr()->getType() == GCABLE_TYPE_STRING) {
    return subject;
  }

  GcableString* str = globalHeap->newString();
  str->str = toStringCpp(subject);
  return Value(str);
}

Value vaiven::sqrt(Value value) {
  if (value.isInt()) {
    return Value(std::sqrt(value.getInt()));
  } else if (value.isDouble()) {
    return Value(std::sqrt(value.getDouble()));
  } else {
    expectedIntOrDouble();
  }
}

Value vaiven::round(Value value) {
  if (value.isInt()) {
    return value;
  } else if (value.isDouble()) {
    return Value(std::round(value.getDouble()));
  } else {
    expectedIntOrDouble();
  }
}

Value vaiven::floor(Value value) {
  if (value.isInt()) {
    return value;
  } else if (value.isDouble()) {
    return Value(std::floor(value.getDouble()));
  } else {
    expectedIntOrDouble();
  }
}

Value vaiven::ceil(Value value) {
  if (value.isInt()) {
    return value;
  } else if (value.isDouble()) {
    return Value(std::ceil(value.getDouble()));
  } else {
    expectedIntOrDouble();
  }
}

Value vaiven::newList() {
  GcableList* list = globalHeap->newList();
  return Value(list);
}

Value vaiven::newListWithSize(int size) {
  GcableList* list = globalHeap->newList();
  list->list.resize(size);
  return Value(list);
}

Value* vaiven::getListContainerUnchecked(GcableList* list) {
  return &list->list[0];
}

Value vaiven::add(Value lhs, Value rhs) {
  if (lhs.isInt()) {
    if (rhs.isInt()) {
      return Value(lhs.getInt() + rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getInt() + rhs.getDouble());
    }
  } else if (lhs.isDouble()) {
    if (rhs.isInt()) {
      return Value(lhs.getDouble() + rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getDouble() + rhs.getDouble());
    }
  } else if (lhs.isPtr() && lhs.getPtr()->getType() == GCABLE_TYPE_STRING) {
    if (rhs.isPtr() && rhs.getPtr()->getType() == GCABLE_TYPE_STRING) {
      GcableString* strleft = (GcableString*) lhs.getPtr();
      GcableString* strright = (GcableString*) rhs.getPtr();
      return addStrUnchecked(strleft, strright);
    }
  }
  expectedStrOrIntOrDouble();
}

Value vaiven::sub(Value lhs, Value rhs) {
  if (lhs.isInt()) {
    if (rhs.isInt()) {
      return Value(lhs.getInt() - rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getInt() - rhs.getDouble());
    }
  } else if (lhs.isDouble()) {
    if (rhs.isInt()) {
      return Value(lhs.getDouble() - rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getDouble() - rhs.getDouble());
    }
  }
  expectedIntOrDouble();
}

Value vaiven::mul(Value lhs, Value rhs) {
  if (lhs.isInt()) {
    if (rhs.isInt()) {
      return Value(lhs.getInt() * rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getInt() * rhs.getDouble());
    }
  } else if (lhs.isDouble()) {
    if (rhs.isInt()) {
      return Value(lhs.getDouble() * rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getDouble() * rhs.getDouble());
    }
  }
  expectedIntOrDouble();
}

Value vaiven::div(Value lhs, Value rhs) {
  double dlhs;
  double drhs;
  if (lhs.isInt()) {
    dlhs = (double) lhs.getInt();
  } else if (lhs.isDouble()) {
    dlhs = lhs.getDouble();
  } else {
    expectedIntOrDouble();
  }
  if (rhs.isInt()) {
    drhs = (double) rhs.getInt();
  } else if (rhs.isDouble()) {
    drhs = rhs.getDouble();
  } else {
    expectedIntOrDouble();
  }

  return Value(dlhs / drhs);
}

Value vaiven::gt(Value lhs, Value rhs) {
  if (lhs.isInt()) {
    if (rhs.isInt()) {
      return Value(lhs.getInt() > rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getInt() > rhs.getDouble());
    }
  } else if (lhs.isDouble()) {
    if (rhs.isInt()) {
      return Value(lhs.getDouble() > rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getDouble() > rhs.getDouble());
    }
  }
  expectedIntOrDouble();
}

Value vaiven::gte(Value lhs, Value rhs) {
  if (lhs.isInt()) {
    if (rhs.isInt()) {
      return Value(lhs.getInt() >= rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getInt() >= rhs.getDouble());
    }
  } else if (lhs.isDouble()) {
    if (rhs.isInt()) {
      return Value(lhs.getDouble() >= rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getDouble() >= rhs.getDouble());
    }
  }
  expectedIntOrDouble();
}

Value vaiven::lt(Value lhs, Value rhs) {
  if (lhs.isInt()) {
    if (rhs.isInt()) {
      return Value(lhs.getInt() < rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getInt() < rhs.getDouble());
    }
  } else if (lhs.isDouble()) {
    if (rhs.isInt()) {
      return Value(lhs.getDouble() < rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getDouble() < rhs.getDouble());
    }
  }
  expectedIntOrDouble();
}

Value vaiven::lte(Value lhs, Value rhs) {
  if (lhs.isInt()) {
    if (rhs.isInt()) {
      return Value(lhs.getInt() <= rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getInt() <= rhs.getDouble());
    }
  } else if (lhs.isDouble()) {
    if (rhs.isInt()) {
      return Value(lhs.getDouble() <= rhs.getInt());
    } else if (rhs.isDouble()) {
      return Value(lhs.getDouble() <= rhs.getDouble());
    }
  }
  expectedIntOrDouble();
}

Value vaiven::addStrUnchecked(GcableString* lhs, GcableString* rhs) {
  GcableString* result = globalHeap->newString();
  result->str = lhs->str + rhs->str;
  return Value(result);
}

Value vaiven::set(Value subject, Value propOrIndex, Value value) {
  if (!subject.isPtr()) {
    expectedInt();
  }

  if (subject.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) subject.getPtr();
    if (propOrIndex.isInt()) {
      listStoreUnchecked(list, propOrIndex.getInt(), value);
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
      return listAccessUnchecked(list, propOrIndex.getInt());
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

Value vaiven::objectAccessChecked(Value subject, string& property) {
  if (!subject.isPtr()) {
    expectedObj();
  }

  if (subject.getPtr()->getType() != GCABLE_TYPE_OBJECT) {
    expectedObj();
  }

  return objectAccessUnchecked((GcableObject*) subject.getPtr(), property);
}

void vaiven::objectStoreChecked(Value subject, string& property, Value value) {
  if (!subject.isPtr()) {
    expectedObj();
  }

  if (subject.getPtr()->getType() != GCABLE_TYPE_OBJECT) {
    expectedObj();
  }

  objectStoreUnchecked((GcableObject*) subject.getPtr(), property, value);
}

Value vaiven::objectAccessUnchecked(GcableObject* object, string& property) {
  if (object->properties.find(property) == object->properties.end()) {
    return Value();
  }
  return object->properties[property];
}

void vaiven::objectStoreUnchecked(GcableObject* object, string& property, Value value) {
  object->properties[property] = value;
}

Value vaiven::listAccessUnchecked(GcableList* list, int index) {
  if (list->list.size() <= index) {
    return Value();
  }
  return list->list[index];
}

void vaiven::listStoreUnchecked(GcableList* list, int index, Value value) {
  if (list->list.size() <= index) {
    list->list.resize(index + 1);
  }
  list->list[index] = value;
}

Value vaiven::cmp(Value a, Value b) {
  return Value((bool) cmpUnboxed(a, b));
}

Value vaiven::inverseCmp(Value a, Value b) {
  return Value((bool) inverseCmpUnboxed(a, b));
}

uint64_t vaiven::cmpUnboxed(Value a, Value b) {
  if (a.isPtr() && b.isPtr()
      && a.getPtr()->getType() == GCABLE_TYPE_STRING
      && b.getPtr()->getType() == GCABLE_TYPE_STRING) {
    return cmpStrUnchecked((GcableString*) a.getPtr(), (GcableString*) b.getPtr());
  } else if (a.isDouble()) {
    if (b.isDouble()) {
      return a.getDouble() == b.getDouble();
    } else if (b.isInt()) {
      return a.getDouble() == b.getInt();
    } else {
      return false;
    }
  } else if (b.isDouble()) {
    if (a.isDouble()) {
      return b.getDouble() == a.getDouble();
    } else if (a.isInt()) {
      return b.getDouble() == a.getInt();
    } else {
      return false;
    }
  }
  return a.getRaw() == b.getRaw();
}

uint64_t vaiven::cmpStrUnchecked(GcableString* a, GcableString* b) {
  return a->str == b->str;
}

uint64_t vaiven::inverseCmpUnboxed(Value a, Value b) {
  return !(bool) cmpUnboxed(a, b);
}

uint64_t vaiven::inverseCmpStrUnchecked(GcableString* a, GcableString* b) {
  return !(bool) cmpStrUnchecked(a, b);
}

string vaiven::toStringCpp(Value subject) {
  if (subject.isPtr() && subject.getPtr()->getType() == GCABLE_TYPE_STRING) {
    return ((GcableString*) subject.getPtr())->str;
  }
  
  if (subject.isPtr() && subject.getPtr()->getType() == GCABLE_TYPE_LIST) {
    GcableList* list = (GcableList*) subject.getPtr();
    string result = "[";
    for (vector<Value>::iterator it = list->list.begin(); it != list->list.end(); ++it) {
      if (it != list->list.begin()) {
        result += ", ";
      }
      result += toStringCpp(*it);
    }
    result += "]";
    return result;
  }
  
  if (subject.isPtr() && subject.getPtr()->getType() == GCABLE_TYPE_OBJECT) {
    GcableObject* object = (GcableObject*) subject.getPtr();
    string result = "{";
    for (unordered_map<string, Value>::iterator it = object->properties.begin(); it != object->properties.end(); ++it) {
      if (it != object->properties.begin()) {
        result += ", ";
      }
      Value val = it->second;
      result += it->first + ":" + toStringCpp(val);
    }
    result += "}";
    return result;
  }
  
  if (subject.isBool() && subject.getBool()) {
    return "true";
  }
  
  if (subject.isBool() && !subject.getBool()) {
    return "false";
  }
  
  if (subject.isVoid()) {
    return "void";
  }

  if (subject.isDouble()) {
    return std::to_string(subject.getDouble());
  }

  return std::to_string(subject.getInt());
}
