#include "heap.h"

#include <algorithm>
#include <cassert>

#include "stack.h"

using namespace vaiven;
using std::min;
using std::max;

// set by interpreter
Heap* vaiven::globalHeap = NULL;

GcableType vaiven::Gcable::getType() {
  return (GcableType) (((GcableType) this->type) & ~GcableMarkBit);
}

bool vaiven::Gcable::isMarked() {
  return (this->type & GcableMarkBit) == GcableMarkBit;
}

bool vaiven::Gcable::mark() {
  if (isMarked()) {
    return false;
  }

  this->type = (GcableType) (((GcableType) this->type) | GcableMarkBit);
  return true;
}

void vaiven::Gcable::unmark() {
  this->type = (GcableType) (((GcableType) this->type) & ~GcableMarkBit);
}


GcableList* vaiven::Heap::newList() {
  if (isFull()) {
    gc();
  }

  GcableList* ptr = new GcableList();
  owned_ptrs.insert(ptr);
  heap_min = min(heap_min, (uint64_t) ptr);
  heap_max = max(heap_max, (uint64_t) ptr);
  return ptr;
}

GcableString* vaiven::Heap::newString() {
  if (isFull()) {
    gc();
  }

  GcableString* ptr = new GcableString();
  owned_ptrs.insert(ptr);
  heap_min = min(heap_min, (uint64_t) ptr);
  heap_max = max(heap_max, (uint64_t) ptr);
  return ptr;
}

GcableObject* vaiven::Heap::newObject() {
  if (isFull()) {
    gc();
  }

  GcableObject* ptr = new GcableObject();
  owned_ptrs.insert(ptr);
  heap_min = min(heap_min, (uint64_t) ptr);
  heap_max = max(heap_max, (uint64_t) ptr);
  return ptr;
}

bool vaiven::Heap::owns(void* ptr) {
  uint64_t addr = (uint64_t) ptr;
  // inside heap and is aligned to 64 bits
  if (addr < heap_min || addr > heap_max || addr & 7 != 1) {
    return false;
  }

  return owned_ptrs.find((Gcable*) ptr) != owned_ptrs.end();
}

bool vaiven::Heap::isFull() {
  return owned_ptrs.size() >= size;
}

void vaiven::Heap::mark(Gcable* ptr) {
  if (!owns(ptr)) {
    return;
  }

  if (ptr->mark()) {
    return;
  }

  switch (ptr->getType()) {
    case GCABLE_TYPE_LIST:
      {
        GcableList* list = (GcableList*) ptr;
        for (vector<Value>::iterator it = list->list.begin(); it != list->list.end(); ++it) {
          mark(it->getPtr());
        }
        break;
      }
    case GCABLE_TYPE_STRING:
      // already marked, nothing to do
      break;
    case GCABLE_TYPE_OBJECT:
      {
        GcableObject* object = (GcableObject*) ptr;
        for (unordered_map<string, Value>::iterator it = object->properties.begin(); it != object->properties.end(); ++it) {
          mark(it->second.getPtr());
        }
      }
  }
}

void vaiven::Heap::sweep() {
  unordered_set<Gcable*>::iterator it = owned_ptrs.begin();
  while (it != owned_ptrs.end()) {
    Gcable* gcable = (Gcable*) *it;
    if (!gcable->isMarked()) {
      switch (gcable->getType()) {
        case GCABLE_TYPE_LIST:
          delete static_cast<GcableList*>(gcable); break;
        case GCABLE_TYPE_STRING:
          delete static_cast<GcableString*>(gcable); break;
        case GCABLE_TYPE_OBJECT:
          delete static_cast<GcableObject*>(gcable); break;
      }
      it = owned_ptrs.erase(it);
    } else {
      gcable->unmark();
      ++it;
    }
  }
}

void vaiven::Heap::free(Gcable* ptr) {
  if (ptr->getType() == GCABLE_TYPE_LIST) {
    delete (GcableList*) ptr;
  } else if (ptr->getType() == GCABLE_TYPE_STRING) {
    delete (GcableString*) ptr;
  } else if (ptr->getType() == GCABLE_TYPE_OBJECT) {
    delete (GcableObject*) ptr;
  }
  owned_ptrs.erase(ptr);
}

void vaiven::Heap::gc() {
  Stack callStack;
  StackFrame frame = callStack.top();
  while (true) {
    for (int i = 0; i < frame.size; ++i) {
      mark((Gcable*) frame.locals[i]);
    }

    if (!frame.hasNext()) {
      break;
    }

    frame = frame.next();
  }

  for (std::deque<Value>::iterator it = interpreterStack.c.begin();
      it != interpreterStack.c.end();
      ++it) {
    mark((Gcable*) it->getPtr());
    
  }

  std::map<string, Value> scopeMap;
  globalScope.fill(scopeMap);

  for (std::map<string, Value>::iterator it = scopeMap.begin();
      it != scopeMap.end();
      ++it) {
    mark((Gcable*) it->second.getPtr());
  }

  sweep();

  size = max((int) owned_ptrs.size() * HEAP_FACTOR, MIN_HEAP_SIZE);
}
