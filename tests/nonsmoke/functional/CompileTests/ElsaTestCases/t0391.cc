// t0391.cc
// using declaration refers to templatized base class overloaded function

template <class T>
struct B {
  void f();
  void f(int) const;

  // original second alternative:
  void f() const;
};

struct A : public B<char> {

  using B<char>::f;

  void g() {
    f();
  }
};
