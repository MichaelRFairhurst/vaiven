#ifndef VAIVEN_VISITOR_HEADER_SCOPE
#define VAIVEN_VISITOR_HEADER_SCOPE

#include <map>
#include <string>
#include <stack>
#include <vector>

using std::map;
using std::string;
using std::stack;
using std::vector;

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
    frames.push(vector<string>());
  }

  void endFrame() {
    for (vector<string>::iterator it = frames.top().begin(); it != frames.top().end(); ++it) {
      currentVars.erase(*it);
    }

    frames.pop();
  }

  void put(string name, V v) {
    if (currentVars.find(name) != currentVars.end()) {
      throw "already in scope";
    }
    
    frames.top().push_back(name);
    currentVars[name] = v;
  }

  void replace(string name, V v) {
    currentVars[name] = v;
  }

  bool contains(string name) {
    return currentVars.find(name) != currentVars.end();
  }

  V get(string name) {
    return currentVars[name];
  }

  private:
  map<string, V> currentVars;

  stack<vector<string> > frames;

};

}

#endif
