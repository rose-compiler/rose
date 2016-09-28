// t0505.cc
// non-type template parameter with type of previous param

template <class T, T i>
int f(T t)
{
  return sizeof(t) + i;
}

int foo()
{
  return f<int,4>(7);
}



