#ifndef HEADER_DOUBLE_DISPATCH
#define HEADER_DOUBLE_DISPATCH

typedef void* (*blankFunc)();

// put in struct for typechecking. Like a checked typedef
struct DDFuncPtrHolder {
  blankFunc ptr;
};

template<typename A, typename B>
DDFuncPtrHolder resolvePtr(A a, B b) {
  DDFuncPtrHolder ddfph;
  ddfph.ptr = static_cast<blankFunc>(&A::handle(b));
  return ddfph;
}

template<typename Self, typename T, typename R>
class DoubleDispatches {
  R doubleDispatch(T& target) {
    DDFuncPtrHolder ddfph = target.resolve(static_cast<Self&>(*this));
    T (Self::*method)(Self& self, T& target)
        = static_cast<T (Self::*)(Self&, T&)>(ddfph.ptr);
    return this->*method(target);
  }
};

template<typename T>
class DoubleDispatchable {
  virtual DDFuncPtrHolder resolve(T& origTarget)=0;
};

#define DOUBLE_DISPATCH(X) virtual DDFuncPtrHolder resolve(X& origTarget) { \
  return resolvePtr(origTarget, *this); \
}

#endif

class Node;
class AdditionExpression;
class SubtractionExpression;

template<typename T>
class NodeAlgorithm : public DoubleDispatches<NodeAlgorithm<T>, Node, T> {
  virtual T handle(AdditionExpression& expr)=0;
  virtual T handle(SubtractionExpression& expr)=0;
};

class IsAdditionAlgorithm : public NodeAlgorithm<bool> {
  virtual bool handle(AdditionExpression& expr) {
    return true;
  }

  virtual bool handle(SubtractionExpression& expr) {
    return false;
  }
};

class Node : public DoubleDispatchable<NodeAlgorithm> {
};

class AdditionExpression : public Node {
  DOUBLE_DISPATCH(NodeAlgorithm)
};

class SubtractionExpression : public Node {
  DOUBLE_DISPATCH(NodeAlgorithm)
};

bool useIt() {
  AdditionExpression expr;
  Node& node = expr;
  IsAdditionAlgorithm algorithm;
  algorithm.doubleDispatch(node);
}
