// t0520.cc
// raise xfailure, and there is a preceding error, but it
// arose on a disambiguation path

template <int n>
struct A {};

int foo()
{
  A < __cause_xfailure(1) > (1);
}
