// t0392.cc
// like t0391.cc but with a member template (!)

template <class T>
struct B {
  template <class U>
  void f(U);
};

struct A : public B<char> {

  using B<char>::f;

  void g() {
    f(1);
  }
};
