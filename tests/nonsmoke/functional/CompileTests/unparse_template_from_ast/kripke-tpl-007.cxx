
namespace N {
  template <typename Ta0>
  struct A {};
}

namespace M {
  template <typename Tb0>
  struct B {};
}

using namespace M;

struct C {
  template<typename Tf0>
  void f() {
    using B = B<Tf0>;
    using X = typename B::X;
    N::A<X> a;
  }
};

