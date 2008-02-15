// here is an example of a program that makes sense and yet does not
// compile with g++ anyway; this also does not compile in elsa/ccparse
// for the same reason.

template <class T>
struct A {
  // NOTE: 'q' is not a typedef; it is a normal variable of some
  // strange type
  typename T::theType q;
};

struct B {
  A<B> ab;                      // no pointer
  typedef int theType;
};

int main() {
  B b;
}
