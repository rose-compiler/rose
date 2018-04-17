// check that delayed instantiation of function members of templatized
// classes works in the presence of inner classes

template <class T>
struct A {
  struct C {                    // inner class makes scope stack of f() interesting
    void foo() {
      // refer to T::x to test A<B> below really instantiates A<B>
      int q = T::x;
      // refer to z to test that function body tcheck is delayed
      int y = z;
    }
    // put Z in C to test that the PartialScopeStack mechanism for
    // delayed template instantiation works
    int z;
  };
};

struct B {                      // nsAString in nsAtomTable.i
  A<B>::C abc;                  // no pointer
  int x;                        // field we need in the body of f()
};

int main() {
  B b;
  b.abc.foo();
}
