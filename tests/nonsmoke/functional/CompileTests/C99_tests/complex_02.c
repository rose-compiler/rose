
// DQ (3/21/2009): The header file "complex.h" is not available in Cygwin.
#if !defined(__CYGWIN__)

// #include "defines.h"
// #include "args.h"
#include <sse_debug.h>
#include <complex.h>

struct IntegerRegisters iregs;
struct FloatRegisters fregs;
unsigned int num_iregs, num_fregs;

#define BUILD_F_COMPLEX(real, imag) \
  ({ __complex__ float __retval = 0; \
     __real__ __retval = (real); \
     __imag__ __retval = (imag); \
     __retval; })

#define BUILD_D_COMPLEX(real, imag) \
  ({ __complex__ double __retval = 0; \
     __real__ __retval = (real); \
     __imag__ __retval = (imag); \
     __retval; })

#define BUILD_LD_COMPLEX(real, imag) \
  ({ __complex__ long double __retval = 0; \
     __real__ __retval = (real); \
     __imag__ __retval = (imag); \
     __retval; })

float passed_f_r, passed_f_i;
__complex__ float
aj_f_times2 (__complex__ float x)
{
  __complex__ float res = 0;

  passed_f_r = xmm_regs[0]._float[0];
  passed_f_i = xmm_regs[0]._float[1];
  __real__ res = (2.0 * __real__ x);
  __imag__ res = (2.0 * __imag__ x);

  return res;
}

double passed_d_r, passed_d_i;
__complex__ double
aj_d_times2 (__complex__ double x)
{
  __complex__ double res = 0;

  passed_d_r = xmm_regs[0]._double[0];
  passed_d_i = xmm_regs[1]._double[0];
  __real__ res = (2.0 * __real__ x);
  __imag__ res = (2.0 * __imag__ x);

  return res;
}

__complex__ long double
aj_ld_times2 (__complex__ long double x)
{
  __complex__ long double res = 0;

  __real__ res = (2.0 * __real__ x);
  __imag__ res = (2.0 * __imag__ x);

  return res;
}

int
main (void)
{
#ifdef CHECK_COMPLEX
  _Complex float fc, fd;
  _Complex double dc, dd;
  _Complex long double ldc, ldd;

  /* complex float: passed in xmm0, returned in xmm0 */
  fc = BUILD_LD_COMPLEX (2.0f, 3.0f);
  clear_float_registers;
  fd = WRAP_RET (aj_f_times2) (fc);
  assert (xmm_regs[0]._float[0] == __real__ fd);
  assert (xmm_regs[0]._float[1] == __imag__ fd);
  clear_float_registers;
  fd = WRAP_CALL (aj_f_times2) (fc);

  assert (passed_f_r == 2.0f && passed_f_i == 3.0f);
  assert (__real__ fd == 4.0f && __imag__ fd == 6.0f);

  /* complex double: passed in xmm0/xmm1, returned in xmm0/xmm1 */
  dc = BUILD_LD_COMPLEX (12.0, 13.0);
  clear_float_registers;
  dd = WRAP_RET (aj_d_times2) (dc);
  assert (xmm_regs[0]._double[0] == __real__ dd);
  assert (xmm_regs[1]._double[0] == __imag__ dd);
  clear_float_registers;
  dd = WRAP_CALL (aj_d_times2) (dc);

  assert (passed_d_r == 12.0 && passed_d_i == 13.0);
  assert (__real__ dd == 24.0 && __imag__ dd == 26.0);

  /* complex long double: passed in memory, returned in st(0)/st(1) */
  ldc = BUILD_LD_COMPLEX (32.0L, 33.0L);
  clear_float_registers;
  ldd = WRAP_RET (aj_ld_times2) (ldc);
  assert (x87_regs[0]._ldouble == __real__ ldd);
  assert (x87_regs[1]._ldouble == __imag__ ldd);

  assert (__real__ ldd == 64.0L && __imag__ ldd == 66.0L);
#endif

  return 0;
}

#endif
