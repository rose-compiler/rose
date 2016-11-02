// unlike t0231.cc, this does compile in g++, but only because the
// '*p' allows for delay in instantiating A<B> during the compilation
// process; since we do not delay instantiating class templates in
// elsa/ccparse, this does NOT compile

template <class T>
struct A {
  // NOTE: 'q' is not a typedef; it is a normal variable of some
  // strange type
  typename T::theType q;
};

struct B {
  A<B> *ab_p;                   // pointer
  typedef int theType;
};

int main() {
  B b;
}
