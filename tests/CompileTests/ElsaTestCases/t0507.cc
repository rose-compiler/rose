// t0507.cc
// template argument goes through two levels of constant variables

int const a = 1;
int const aa = a;

template <int i>
int f()
{
  return i;
}

void foo()
{
  f<a>();          // one level
  f<aa>();         // two levels
}
