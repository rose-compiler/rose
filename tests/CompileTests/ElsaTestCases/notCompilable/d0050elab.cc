// this is the elaborated version of d0050.cc; here, I put in the the
// copy assign operator but not the copy assignment operator, since
// those are the conditions under which this test originally exposed a
// bug; this was elaborated using the prettyPrinter but then hand
// edited.
struct B {
  inline B(B const &__other) {}
};
struct C:B {
  C &operator=(/*m: struct C & */ C const &s);
  C &operator=(/*m: struct C & */ B const &t);
};
struct D:C {
  inline D &operator=(/*m: struct D & */ D const &__other) {
    this->C::operator=(__other);
    return *this;
  }
};
