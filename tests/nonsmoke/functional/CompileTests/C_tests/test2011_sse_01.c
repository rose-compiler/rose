// Example of SSE useage in Intel and GNU compilers (from Stephen Guzik)
// You have to define "INTEL_COMPILER" to get it to compile with icpc.

#include <iostream>
#include <xmmintrin.h>  // SSE

union CH128_t
{
  __m128 m;
  float f[4];
};
  
int main()
{
  float __attribute__ ((aligned (16))) data[8] =
    { 1.5f, 2.f, 3.5f, 4.f, -2.f, 2.5f, 3.f, 4.5f};
  CH128_t a, b, c;
  a.m = _mm_load_ps(data);
  b.m = _mm_load_ps(data+4);

#ifdef INTEL_COMPILER
  c.m = _mm_add_ps(a.m, b.m);
#else
  c.m = a.m + b.m;                                 // Should generate addps
#endif

  std::cout << __alignof__(CH128_t) << std::endl;  // A confirmation
  std::cout << c.f[0] << ' ' << c.f[1] << ' ' << c.f[2] << ' ' << c.f[3]
            << std::endl;
}
