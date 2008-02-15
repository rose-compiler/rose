// t0158.cc
// define a function that was declared inside a namespace

namespace N {
  int f();
  int x;
}

int N::f()
{
  // I can't find where the standard specifies that the body
  // of N::f can see all of N, but I think it's safe to assume
  // that it can, similar to how class methods work
  return x;
}
