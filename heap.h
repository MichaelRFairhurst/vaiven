#ifndef VAIVEN_VISITOR_HEADER_HEAP
#define VAIVEN_VISITOR_HEADER_HEAP

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

#include "scope.h"
#include "value.h"
#include "util.h"

using std::unordered_map;
using std::unordered_set;
using std::string;
using std::vector;

namespace vaiven {

// make these bits so they can be stored in shapes bitmaps
enum GcableType {
  GCABLE_TYPE_LIST = 1,
  GCABLE_TYPE_STRING = 2,
  GCABLE_TYPE_OBJECT = 4,

  Internal_ForceMyEnumIntSize = 1 << 30
};

const int GcableMarkBit = 8;
const int HEAP_FACTOR = 2;
const int MIN_HEAP_SIZE = 65536;

struct Gcable {
  public:
  Gcable(GcableType type) : type(type) {};
  GcableType getType();
  bool isMarked();
  bool mark();
  void unmark();

  private:
  GcableType type;
};

struct GcableList : public Gcable {
  public:
  GcableList() : Gcable(GCABLE_TYPE_LIST) {};

  vector<Value> list;
};

struct GcableString : public Gcable {
  public:
  GcableString() : Gcable(GCABLE_TYPE_STRING) {};
  GcableString(string str) : Gcable(GCABLE_TYPE_STRING), str(str) {};

  string str;
};

struct GcableObject : public Gcable {
  public:
  GcableObject() : Gcable(GCABLE_TYPE_OBJECT) {};

  unordered_map<string, Value> properties;
};

class Heap {
  public:
  Heap(stack_with_container<Value>& interpreterStack, Scope<Value>& globalScope)
    : size(MIN_HEAP_SIZE), heap_min(0xFFFFFFFFFFFFFFFF), heap_max(0),
    interpreterStack(interpreterStack), globalScope(globalScope) {};
  int size;
  uint64_t heap_min;
  uint64_t heap_max;

  // for roots
  Scope<Value>& globalScope;
  stack_with_container<Value>& interpreterStack;

  GcableList* newList();
  GcableString* newString();
  GcableObject* newObject();
  void free(Gcable* ptr);
  bool isFull();
  bool owns(void* ptr);
  void mark(Gcable* ptr);
  void sweep();
  void gc();

  // TODO this is TERRIBLE
  unordered_set<Gcable*> owned_ptrs;
};

extern Heap* globalHeap;

}

#endif
