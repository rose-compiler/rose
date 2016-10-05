// another version of t0210.cc; attempt to cause a problem by calling
// f() in B in a function body before getting to x

template <class T>
struct A {
  int f();
};

template <class T>
int A<T>::f()
{
  T t;                          // no pointer
  t.x = 0;
  return 0;
}

struct B {
  A<B> ab;                      // no pointer
  B() {
    // NOTE: still too soon to instantiate f()!  this will be done on
    // the second pass throught the class members so we are OK.
    int y = ab.f();
  }

  int x;                        // field we need in the body of f()
};

int main() {
  B b;
  b.ab.f();
  return 0;
}
