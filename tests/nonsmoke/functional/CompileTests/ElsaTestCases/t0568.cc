// t0568.cc
// declaration after use...

// GCC and ICC both accept this.

// I think it is invalid because the point of instantiation
// of the template functions is too soon to see Min
// [14.6.1.1p1, 14.7.1p2].  But, Elsa will support it anyway.

template <class T>
struct A {
  int capacity;
  void f (int nsz);
  inline void g (int nsz);
  inline void h (int nsz)
  {
    Min(this->capacity, nsz);
  }
};

template <class T>
void A<T>::f(int nsz)
{
  Min(this->capacity, nsz);
}

template <class T>
void A<T>::g(int nsz)
{
  Min(this->capacity, nsz);
}

template void A<int>::f(int nsz);

void foo()
{
  A<int> a;
  a.f(3);
  a.g(3);
  a.h(3);
}

template <class T>
T Min (const T & x, const T & y, int z = 15)
{
  return x + z;
}
