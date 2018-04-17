// t0469.cc
// another use of a dependent qualified name

struct C {};

template <class T>
void f(T t) {
  C& c;
  c.T::g();
}
