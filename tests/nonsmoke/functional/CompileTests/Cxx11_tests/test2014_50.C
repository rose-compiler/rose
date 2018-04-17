
// DQ (4/5/2017): These tests can't be supported using Intel V14
#if !(defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 1400))

// template<char...> OutputType operator "" _suffix();
long double operator "" _w(long double);
long double operator "" _w(long double) { return 0.0; }

#endif
