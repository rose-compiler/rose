// g0008.cc
// test __FUNCTION__ etc. in C++ mode

void f()
{
  char const *b = __FUNCTION__;
  char const *c = __PRETTY_FUNCTION__;

  //ERROR(1):   char const *d = something_else;             // undefined
  //ERROR(3):   char const *f = "x" __FUNCTION__;           // can't concat
  //ERROR(4):   char const *g = "x" __PRETTY_FUNCTION__;    // can't concat

  // 2005-08-10: It seems that GCC does allow this in C++,
  // even though it is a C99 feature.
  char const *h = __func__;                   // not in (ANSI) C++
}
