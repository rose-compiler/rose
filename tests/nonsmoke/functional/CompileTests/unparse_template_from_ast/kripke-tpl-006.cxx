
namespace N {
  template <typename Ta0>
  struct A {};
}

namespace M {
  template <typename Tb0>
  struct B {};
}

struct C {
  template<typename Tf0>
  void f() {
    using B = M::B<Tf0>;
    using X = typename B::X;
    N::A<X> a;
  }
};

