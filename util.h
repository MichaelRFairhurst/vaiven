#ifndef VAIVEN_VISITOR_HEADER_UTIL
#define VAIVEN_VISITOR_HEADER_UTIL

#include <stack>

template<typename T>
class stack_with_container : public std::stack<T> {
  public:
    using std::stack<T>::c; // expose the container
};

#endif
