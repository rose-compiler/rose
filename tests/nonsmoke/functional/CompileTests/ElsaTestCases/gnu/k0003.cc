// gcc 3.4 __builtin_* math functions

// http://gcc.gnu.org/onlinedocs/gcc-3.4.3/gcc/Other-Builtins.html
// though that page does not document "modff" ...

// ERR-MATCH: __builtin_f

inline double
abs(double __x)
{ return __builtin_fabs(__x); }

inline float
abs(float __x)
{ return __builtin_fabsf(__x); }

inline long double
abs(long double __x)
{ return __builtin_fabsl(__x); }


inline float
acos(float __x)
{ return __builtin_acosf(__x); }

inline long double
acos(long double __x)
{ return __builtin_acosl(__x); }

inline double acos(double __x)
{
    return __builtin_acos(__x);
}

inline float
asin(float __x)
{ return __builtin_asinf(__x); }

inline long double
asin(long double __x)
{ return __builtin_asinl(__x); }

inline double asin(double __x)
{ return __builtin_asin(__x); }

inline float
atan(float __x)
{ return __builtin_atanf(__x); }

inline long double
atan(long double __x)
{ return __builtin_atanl(__x); }

inline double atan(double __x)
{ return __builtin_atan(__x); }

inline float
atan2(float __y, float __x)
{ return __builtin_atan2f(__y, __x); }

inline long double
atan2(long double __y, long double __x)
{ return __builtin_atan2l(__y, __x); }

inline double
atan2(double __y, double __x)
{ return __builtin_atan2(__y, __x); }

inline float
ceil(float __x)
{ return __builtin_ceilf(__x); }

inline long double
ceil(long double __x)
{ return __builtin_ceill(__x); }

inline double ceil(double __x)
{ return __builtin_ceil(__x); }

inline float
cos(float __x)
{ return __builtin_cosf(__x); }

inline long double
cos(long double __x)
{ return __builtin_cosl(__x); }

inline double cos(double __x)
{ return __builtin_cos(__x); }

inline float
cosh(float __x)
{ return __builtin_coshf(__x); }

inline long double
cosh(long double __x)
{ return __builtin_coshl(__x); }

inline double cosh(double __x)
{ return __builtin_cosh(__x); }

inline float
exp(float __x)
{ return __builtin_expf(__x); }

inline long double
exp(long double __x)
{ return __builtin_expl(__x); }

inline double exp(double __x)
{ return __builtin_exp(__x); }

inline float
fabs(float __x)
{ return __builtin_fabsf(__x); }

inline long double
fabs(long double __x)
{ return __builtin_fabsl(__x); }

inline double fabs(double __x)
{ return __builtin_fabs(__x); }

inline float
floor(float __x)
{ return __builtin_floorf(__x); }

inline long double
floor(long double __x)
{ return __builtin_floorl(__x); }

inline double floor(double __x)
{ return __builtin_floor(__x); }

inline float
fmod(float __x, float __y)
{ return __builtin_fmodf(__x, __y); }

inline long double
fmod(long double __x, long double __y)
{ return __builtin_fmodl(__x, __y); }

inline float
frexp(float __x, int* __exp)
{ return __builtin_frexpf(__x, __exp); }

inline long double
frexp(long double __x, int* __exp)
{ return __builtin_frexpl(__x, __exp); }

inline double frexp(double __x, int* __exp)
{ return __builtin_frexp(__x, __exp); }

inline float
ldexp(float __x, int __exp)
{ return __builtin_ldexpf(__x, __exp); }

inline long double
ldexp(long double __x, int __exp)
{ return __builtin_ldexpl(__x, __exp); }

inline double ldexp(double __x, int __exp)
{ return __builtin_ldexp(__x, __exp); }

inline float
log(float __x)
{ return __builtin_logf(__x); }

inline long double
log(long double __x)
{ return __builtin_logl(__x); }

inline double log(double __x)
{ return __builtin_log(__x); }

inline float
log10(float __x)
{ return __builtin_log10f(__x); }

inline long double
log10(long double __x)
{ return __builtin_log10l(__x); }

inline double log10(double __x)
{ return __builtin_log10(__x); }

inline float
modf(float __x, float* __iptr)
{ return __builtin_modff(__x, __iptr); }

inline long double
modf(long double __x, long double* __iptr)
{ return __builtin_modfl(__x, __iptr); }

inline float
pow(float __x, float __y)
{ return __builtin_powf(__x, __y); }

inline long double
pow(long double __x, long double __y)
{ return __builtin_powl(__x, __y); }

inline float
sin(float __x)
{ return __builtin_sinf(__x); }

inline long double
sin(long double __x)
{ return __builtin_sinl(__x); }

inline double sin(double __x)
{ return __builtin_sin(__x); }

inline float
sinh(float __x)
{ return __builtin_sinhf(__x); }

inline long double
sinh(long double __x)
{ return __builtin_sinhl(__x); }

inline double sinh(double __x)
{ return __builtin_sinh(__x); }

inline float
sqrt(float __x)
{ return __builtin_sqrtf(__x); }

inline long double
sqrt(long double __x)
{ return __builtin_sqrtl(__x); }

inline double sqrt(double __x)
{ return __builtin_sqrt(__x); }

inline float
tan(float __x)
{ return __builtin_tanf(__x); }

inline long double
tan(long double __x)
{ return __builtin_tanl(__x); }

inline double tan(double __x)
{ return __builtin_tan(__x); }

inline float
tanh(float __x)
{ return __builtin_tanhf(__x); }

inline long double
tanh(long double __x)
{ return __builtin_tanhl(__x); }

inline double tanh(double __x)
{ return __builtin_tanh(__x); }

