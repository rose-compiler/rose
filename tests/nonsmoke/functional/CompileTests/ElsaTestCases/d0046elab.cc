// this is the elaborated version of d0046.cc; that is, I put in the
// copy ctors, but not the copy assign operator, since those are the
// conditions under which this test originally exposed a bug; this was
// elaborated using the prettyPrinter but then hand edited.
typedef int A;
template <class T>
struct B {
  inline B(B<T> const &__other) {
  }
};
template <class T2>
struct B<T2 *> {
  inline int f(/*m: template <class T2> struct B & */ T2 */*anon*/) {
    return A(3);
  }
  inline B(B<T2*> const &__other) {
  }
};
