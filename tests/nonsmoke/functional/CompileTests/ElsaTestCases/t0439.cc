// t0439.cc
// use a default argument in an explicit specialization spec (!)

template <class T = int>
struct A {};

template <>
struct A</*int*/> {      // !!
  int x;
};

int f()
{
  A<> a;
  a.x = 5;
  return a.x;
}
