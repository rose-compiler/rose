// g0028.cc
// from gcc-3.4.3 <limits> header

static float infinity1() throw()
{ return __builtin_huge_valf (); }

static double infinity2() throw()
{ return __builtin_huge_val(); }

static long double infinity3() throw()
{ return __builtin_huge_vall (); }

static double quiet_NaN() throw()
{ return __builtin_nan (""); }
