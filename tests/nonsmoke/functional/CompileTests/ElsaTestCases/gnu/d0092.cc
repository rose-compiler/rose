// some builtins that I ran into

// # 441 "/usr/include/bits/mathinline.h"
__inline double fabs (double __x) throw () { return __builtin_fabs (__x); }
__inline float fabsf (float __x) throw () { return __builtin_fabsf (__x); }
__inline long double fabsl (long double __x) throw () { return __builtin_fabsl (__x); }
__inline long double __fabsl (long double __x) throw () { return __builtin_fabsl (__x); }
