// t0518.cc
// raise xfailure and there are no preceding errors

void foo()
{
  __cause_xfailure();
}
