// another version of t0210.cc where in the body of f() we have a
// stack-allocated T instead of a pointer to one

template <class T>
struct A {
  void f();
};

template <class T>
void A<T>::f()
{
  T t;                          // NOTE: no pointer
  t.x = 0;
}

struct B {
  A<B> ab;                      // no pointer
  int x;                        // field we need in the body of f()
};

int main() {
  B b;
  b.ab.f();
  return 0;
}
