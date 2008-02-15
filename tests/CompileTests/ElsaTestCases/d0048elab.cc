// d0048elab.cc:19:11: error: ambiguous overload, no function is better than all others
struct B {
  inline B(struct B const &__other) {
  }
  inline struct B &operator=(/*m: struct B & */ struct B const &__other) {
    return (* (this));
  }
};
struct C:B {
  struct C &operator=(/*m: struct C & */ struct C const &s);
  struct C &operator=(/*m: struct C & */ struct B const &t);
  inline C(struct C const &__other):B ( (__other)) {
  }
};
struct D:C {
  inline D(struct D const &__other):C ( (__other)) {
  }
  inline struct D &operator=(/*m: struct D & */ struct D const &__other) {
    this->C::operator=(__other); // NOTE: this line doesn't pretty print right so fixed by hand
    return (* (this));
  }
};
