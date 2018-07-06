
class A {};
class B {};

template <typename... T>
struct X {
  // if an instantiation of X is only seen through a reference:
  //     the `a_field` for `sizeT` has the flag `has_initializer` to true
  //     BUT `initializer` is NULL
  const int sizeT = sizeof...(T);
};

// `foo` is correct

void foo() {
  X<A> x;
  const int sizeT = x.sizeT;
}

// `bar` fails
//    - a reference to X<A> instead of X<B> would work

void bar(X<B> & x) {
  const int sizeT = x.sizeT;
}

