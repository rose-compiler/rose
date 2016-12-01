// This is the template function for member functions.
template <int T> void test() {}

#if 0
// DQ (8/12/2013): floating point values can't be used as templae parameters in C++.
// This is the template function for member data.
template <double T> void test() {}
#endif

void foo()
   {
     const int    x = 1;
     const double y = -1.0;

     test<x>();

  // This is not allowed for GNU g++ version 6.x
  // #if __cplusplus < 201103L
#if (__GNUC__ < 6)
     test<y>();
#endif
   }

