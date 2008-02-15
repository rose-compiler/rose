// t0519.cc
// raise xfailure, but a preceding error reduces severity

void foo()
{
  x;   // preceding error
  __cause_xfailure();
}
