// Same as t0210.cc except that the body of the function member of the
// templatized class is pulled out-of-line

template <class T>
struct A {
  void f();
};

template <class T>
void A<T>::f()
{
  T *t;                         // pointer
  t->x = 0;
}

struct B {                      // nsAString in nsAtomTable.i
  A<B> ab;                      // no pointer
  int x;                        // field we need in the body of f()
};

int main() {
  B b;
  b.ab.f();
  return 0;
}
