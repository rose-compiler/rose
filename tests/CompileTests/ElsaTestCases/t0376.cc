// t0376.cc
// E.A.: namespace lookup returning set

namespace N {
  struct A {};

  A& operator<<(A&, int);
}

N::A& operator<<(N::A&, char*);

template <class T>
struct B {
  B(T t) {
    N::A a;
    a << t;
  }
};

using namespace N;

void f() {
  B<int> b(1);
}
