// t0575.cc
// explicit spec of member template func

struct A {
  template <class T>
  void f(const T &);

  template <class T>
  static void g(const T &);

  int d;
};

template <>
void A::f(const int &)
{
  d;   // ok
}

template <>
void A::g(const int &)
{
  //ERROR(1): d;    // method is static
}
