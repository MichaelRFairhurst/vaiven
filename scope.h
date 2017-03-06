#ifndef VAIVEN_VISITOR_HEADER_SCOPE
#define VAIVEN_VISITOR_HEADER_SCOPE

#include <map>
#include <string>
#include <stack>
#include <unordered_set>

using std::map;
using std::string;
using std::stack;
using std::unordered_set;

namespace vaiven {

template<typename V>
class Scope;

template<typename V>
class ScopeFrame {

  public:
  ScopeFrame(Scope<V>& scope) : scope(scope) {
    scope.newFrame();
  }

  ~ScopeFrame() {
    scope.endFrame();
  }

  private:
  Scope<V>& scope;
};

template<typename V>
class Scope {
  public:
  Scope() {
    newFrame();
  }

  void newFrame() {
    frames.push(unordered_set<string>());
  }

  void endFrame() {
    for (unordered_set<string>::iterator it = frames.top().begin(); it != frames.top().end(); ++it) {
      currentVars.erase(*it);
    }

    frames.pop();
  }

  void put(string name, V v) {
    if (currentVars.find(name) != currentVars.end()) {
      throw "already in scope";
    }
    
    frames.top().insert(name);
    currentVars[name] = v;
  }

  void replace(string name, V v) {
    currentVars[name] = v;
  }

  bool contains(string name) {
    return currentVars.find(name) != currentVars.end();
  }

  bool inHigherScope(string name) {
    return frames.top().find(name) == frames.top().end();
  }

  V get(string name) {
    return currentVars[name];
  }

  void fill(map<string, V>& toFill) {
    toFill.insert(currentVars.begin(), currentVars.end());
  }

  private:
  map<string, V> currentVars;

  stack<unordered_set<string> > frames;

};

}

#endif
