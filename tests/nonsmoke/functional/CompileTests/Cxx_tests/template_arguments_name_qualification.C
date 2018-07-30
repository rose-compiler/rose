

template <template <typename> typename TPL>
struct A {
  using type = A;
};

template <typename T>
struct B {
  using type = typename A<T::template tpl>::type;
};

namespace N {
  struct C {
    template <typename>
    struct tpl {};
  };
}

B<N::C> x;

