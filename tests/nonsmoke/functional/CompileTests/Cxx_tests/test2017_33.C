
#if 0
// For GNU 5.1 ROSE is not configured to be in C++11 mode by default.
#if __cplusplus >= 201103L
#error "Testing in C++11 mode."
#else
#error "NOT Testing in C++11 mode."
#endif
#endif

#include <fenv.h>

void setup_fpu()
{
#ifdef HAVE_FEDISABLEEXCEPT
  fedisableexcept(FE_ALL_EXCEPT);
#endif

  /* Set FPU rounding mode to "round-to-nearest" */
  fesetround(FE_TONEAREST);

  /*
    x86 (32-bit) requires FPU precision to be explicitly set to 64 bit
    (double precision) for portable results of floating point operations.
    However, there is no need to do so if compiler is using SSE2 for floating
    point, double values will be stored and processed in 64 bits anyway.
  */
#if defined(__i386__) && !defined(__SSE2_MATH__)
#if defined(_WIN32)
#if !defined(_WIN64)
  _control87(_PC_53, MCW_PC);
#endif /* !_WIN64 */
#else /* !_WIN32 */
  fpu_control_t cw;
  _FPU_GETCW(cw);
  cw= (cw & ~_FPU_EXTENDED) | _FPU_DOUBLE;
  _FPU_SETCW(cw);
#endif /* _WIN32 && */
#endif /* __i386__ */

}
