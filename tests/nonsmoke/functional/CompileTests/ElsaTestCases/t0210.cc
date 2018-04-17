// this is an example that Scott cooked up to demonstrate that greedy
// instantiation of function members of class templates doesn't work

// if function members of class templates are instantiated greedily
// then this causes A to be instantiated with T=B and, since the
// definition of f() has been seen (in this example, it is inlined),
// the body of f().  However, since we have not finished typechecking
// B, the member B::x (which is t->x in f() ) does not exist yet, and
// typechecking fails.

template <class T>
struct A {
  void f() {
    T *t;// = new T;               // pointer
    t->x = 0;
  }
};

struct B {                      // nsAString in nsAtomTable.i
  A<B> ab;                      // no pointer
  int x;                        // field we need in the body of f()
};

int main() {
  B b;
  b.ab.f();
  return 0;
}
