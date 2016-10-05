// t0055.cc
// member initializer where member is a templatized base class
      
template <class T>
class A {
public:
  int f();
  A(T t);
  int g();
};

class B : A<int> {
public:
  B() : A<int>(5) {}
};


// variant using an explicit specialization
template <>
class A<float> {};

struct D : A<float> {
  D() : A<float>() {}

  void f()
  {
    A<float> a;
  }
};
