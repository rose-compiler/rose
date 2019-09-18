/* This file is not in use since the original .h.in version is being used by cmake,
   which in turn calls the shell script create_system_headers to generate .h from .h.in.
*/
/* JJW (12/8/2008): Do not include these definitions for EDG 3.10 */
/* TV (06/2019): use to be predicated on ROSE_USE_NEW_EDG_INTERFACE.i
 *    FIXME: Is it really the case that no pre-include are needed for CMAKE
 */
#if 0

/* Must use C style comments so that "--edg:old_c" options will work! */
/* DQ (7/13/2006): Undefine these before defining them to avoid warnings. */
/* DQ (12/23/2006): Let EDG define this if possible, but we reset it for 64 bit systems
   where either EDG does not get it correct or we don't setup EDG correctly!
#undef __SIZE_TYPE__ 
*/
#undef __VERSION__

// $REPLACE_ME_WITH_MACRO_DEFINITIONS
#define __DBL_MIN_EXP__ (-1021)
#define __FLT_MIN__ 1.17549435e-38F
#define __DEC64_DEN__ 0.000000000000001E-383DD
#define __CHAR_BIT__ 8
#define __WCHAR_MAX__ 2147483647
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 1
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 1
#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 1
#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
#define __FLT_EVAL_METHOD__ 2
#define __unix__ 1
#define __DBL_MIN_10_EXP__ (-307)
#define __FINITE_MATH_ONLY__ 0
#define __DEC64_MAX_EXP__ 384
#define __SHRT_MAX__ 32767
#define __LDBL_MAX__ 1.18973149535723176502e+4932L
#define __UINTMAX_TYPE__ long long unsigned int
#define __linux 1
#define __DEC32_EPSILON__ 1E-6DF
#define __unix 1
#define __LDBL_MAX_EXP__ 16384
#define __linux__ 1
#define __SCHAR_MAX__ 127
#define __DBL_DIG__ 15
#define _FORTIFY_SOURCE 2
#define __SIZEOF_INT__ 4
#define __SIZEOF_POINTER__ 4
#define __USER_LABEL_PREFIX__ 
#define __LDBL_HAS_INFINITY__ 1
#define __FLT_EPSILON__ 1.19209290e-7F
#define __LDBL_MIN__ 3.36210314311209350626e-4932L
#define __DEC32_MAX__ 9.999999E96DF
#define __SIZEOF_LONG__ 4
#define __DECIMAL_DIG__ 21
#define __gnu_linux__ 1
#define __LDBL_HAS_QUIET_NAN__ 1
#define __GXX_RTTI 1
#define __FLT_HAS_DENORM__ 1
#define __SIZEOF_LONG_DOUBLE__ 12
#define __DBL_MAX__ 1.7976931348623157e+308
#define __DBL_HAS_INFINITY__ 1
#define __DEC32_MIN_EXP__ (-95)
#define __LDBL_HAS_DENORM__ 1
#define __DEC128_MAX__ 9.999999999999999999999999999999999E6144DL
#define __DEC32_MIN__ 1E-95DF
#define __DEPRECATED 1
#define __DBL_MAX_EXP__ 1024
#define __DEC128_EPSILON__ 1E-33DL
#define __LONG_LONG_MAX__ 9223372036854775807LL
#define __SIZEOF_SIZE_T__ 4
#define __SIZEOF_WINT_T__ 4
#define __GXX_ABI_VERSION 1002
#define __FLT_MIN_EXP__ (-125)
#define __DBL_MIN__ 2.2250738585072014e-308
#define __FLT_MIN_10_EXP__ (-37)
#define __DECIMAL_BID_FORMAT__ 1
#define __DEC128_MIN__ 1E-6143DL
#define __REGISTER_PREFIX__ 
#define __DBL_HAS_DENORM__ 1
#define __NO_INLINE__ 1
#define __i386 1
#define __FLT_MANT_DIG__ 24
#define __VERSION__ "4.3.3"
#define __DEC64_EPSILON__ 1E-15DD
#define __DEC128_MIN_EXP__ (-6143)
#define __i486__ 1
#define unix 1
#define __i386__ 1
#define __SIZE_TYPE__ unsigned int
#define __DEC32_DEN__ 0.000001E-95DF
#define __ELF__ 1
#define __FLT_RADIX__ 2
#define __LDBL_EPSILON__ 1.08420217248550443401e-19L
#define __SIZEOF_PTRDIFF_T__ 4
#define __FLT_HAS_QUIET_NAN__ 1
#define __FLT_MAX_10_EXP__ 38
#define __LONG_MAX__ 2147483647L
#define __FLT_HAS_INFINITY__ 1
#define __DEC64_MAX__ 9.999999999999999E384DD
#define __DEC64_MANT_DIG__ 16
#define __DEC32_MAX_EXP__ 96
#define linux 1
#define __DEC128_DEN__ 0.000000000000000000000000000000001E-6143DL
#define __EXCEPTIONS 1
#define __LDBL_MANT_DIG__ 64
#define __DBL_HAS_QUIET_NAN__ 1
#define __WCHAR_TYPE__ int
#define __SIZEOF_FLOAT__ 4
#define __DEC64_MIN_EXP__ (-383)
#define __FLT_DIG__ 6
#define __INT_MAX__ 2147483647
#define __i486 1
#define __FLT_MAX_EXP__ 128
#define __DBL_MANT_DIG__ 53
#define __DEC64_MIN__ 1E-383DD
#define __WINT_TYPE__ unsigned int
#define __SIZEOF_SHORT__ 2
#define __LDBL_MIN_EXP__ (-16381)
#define __LDBL_MAX_10_EXP__ 4932
#define __DBL_EPSILON__ 2.2204460492503131e-16
#define __SIZEOF_WCHAR_T__ 4
#define __DEC_EVAL_METHOD__ 2
#define __INTMAX_MAX__ 9223372036854775807LL
#define __FLT_DENORM_MIN__ 1.40129846e-45F
#define __FLT_MAX__ 3.40282347e+38F
#define __SIZEOF_DOUBLE__ 8
#define __INTMAX_TYPE__ long long int
#define i386 1
#define __DEC128_MAX_EXP__ 6144
#define __DEC32_MANT_DIG__ 7
#define __DBL_MAX_10_EXP__ 308
#define __LDBL_DENORM_MIN__ 3.64519953188247460253e-4951L
#define __STDC__ 1
#define __PTRDIFF_TYPE__ int
#define __DEC128_MANT_DIG__ 34
#define __LDBL_MIN_10_EXP__ (-4931)
#define __SIZEOF_LONG_LONG__ 8
#define __LDBL_DIG__ 18
#define __GNUC_GNU_INLINE__ 1
#define _GNU_SOURCE 1

/* DQ (7/11/2009): fix emacs coloring... with a '$' */

/* Turn on use of restrict in EDG front-end using --edg:restrict */
#ifdef __GNUC__
/* for GNU g++ */
#define restrict __restrict__
#endif

/* DQ (6/12/2005): Permit this to be optionally commented out because it makes it hard to
    look at the AST merging graphs. The ROSE-IR graphs will now filter nodes associated
    with declarations from this file (marked as front-end specific).
*/
#if 0
#define SKIP_ROSE_BUILTIN_DECLARATIONS
#endif

/* This macro can be defined using "-DSKIP_ROSE_BUILTIN_DECLARATIONS" to avoid ROSE
   builtin functions required for compatability with the user selected backend compiler 
*/
#ifndef SKIP_ROSE_BUILTIN_DECLARATIONS

#ifndef ROSE_LANGUAGE_MODE
  #error "Macro ROSE_LANGUAGE_MODE should have been defined ROSE_LANGUAGE_MODE == 0 for C and C99 and ROSE_LANGUAGE_MODE == 1 for C++ (CUDA == 2, OpenCL == 3)"
#endif

/* PC (9/25/2006): Define __builtin_va_start to the EDG expected symbol
   __builtin_stdarg_start */
#define __builtin_va_start __builtin_stdarg_start

/* Outside strict ISO C mode (-ansi, -std=c89 or -std=c99), the functions _exit, alloca,
    bcmp, bzero, dcgettext, dgettext, dremf, dreml, drem, exp10f, exp10l, exp10, ffsll,
    ffsl, ffs, fprintf_unlocked, fputs_unlocked, gammaf, gammal, gamma, gettext, index,
    isascii, j0f, j0l, j0, j1f, j1l, j1, jnf, jnl, jn, mempcpy, pow10f, pow10l, pow10,
    printf_unlocked, rindex, scalbf, scalbl, scalb, signbit, signbitf, signbitl,
    significandf, significandl, significand, sincosf, sincosl, sincos, stpcpy, strdup,
    strfmon, toascii, y0f, y0l, y0, y1f, y1l, y1, ynf, ynl and yn may be handled as
    built-in functions. All these functions have corresponding versions prefixed with
    __builtin_, which may be used even in strict C89 mode.
*/
/* extern void _exit ( int Status); */

/* The ISO C99 functions _Exit, acoshf, acoshl, acosh, asinhf, asinhl, asinh, atanhf,
    atanhl, atanh, cabsf, cabsl, cabs, cacosf, cacoshf, cacoshl, cacosh, cacosl, cacos,
    cargf, cargl, carg, casinf, casinhf, casinhl, casinh, casinl, casin, catanf, catanhf,
    catanhl, catanh, catanl, catan, cbrtf, cbrtl, cbrt, ccosf, ccoshf, ccoshl, ccosh,
    ccosl, ccos, cexpf, cexpl, cexp, cimagf, cimagl, cimag, conjf, conjl, conj, copysignf,
    copysignl, copysign, cpowf, cpowl, cpow, cprojf, cprojl, cproj, crealf, creall, creal,
    csinf, csinhf, csinhl, csinh, csinl, csin, csqrtf, csqrtl, csqrt, ctanf, ctanhf,
    ctanhl, ctanh, ctanl, ctan, erfcf, erfcl, erfc, erff, erfl, erf, exp2f, exp2l, exp2,
    expm1f, expm1l, expm1, fdimf, fdiml, fdim, fmaf, fmal, fmaxf, fmaxl, fmax, fma, fminf,
    fminl, fmin, hypotf, hypotl, hypot, ilogbf, ilogbl, ilogb, imaxabs, isblank, iswblank,
    lgammaf, lgammal, lgamma, llabs, llrintf, llrintl, llrint, llroundf, llroundl,
    llround, log1pf, log1pl, log1p, log2f, log2l, log2, logbf, logbl, logb, lrintf,
    lrintl, lrint, lroundf, lroundl, lround, nearbyintf, nearbyintl, nearbyint,
    nextafterf, nextafterl, nextafter, nexttowardf, nexttowardl, nexttoward, remainderf,
    remainderl, remainder, remquof, remquol, remquo, rintf, rintl, rint, roundf, roundl,
    round, scalblnf, scalblnl, scalbln, scalbnf, scalbnl, scalbn, snprintf, tgammaf,
    tgammal, tgamma, truncf, truncl, trunc, vfscanf, vscanf, vsnprintf and vsscanf are
    handled as built-in functions except in strict ISO C90 mode (-ansi or -std=c89). 
*/
/* extern void _Exit ( int Status); */
double      __builtin_copysign  (double __builtin__x, double __builtin__y);
float       __builtin_copysignf (float __builtin__x, float __builtin__y);
long double __builtin_copysignl (long double __builtin__x, long double __builtin__y);

/* There are also built-in versions of the ISO C99 functions acosf, acosl, asinf, asinl,
    atan2f, atan2l, atanf, atanl, ceilf, ceill, cosf, coshf, coshl, cosl, expf, expl,
    fabsf, fabsl, floorf, floorl, fmodf, fmodl, frexpf, frexpl, ldexpf, ldexpl, log10f,
    log10l, logf, logl, modfl, modf, powf, powl, sinf, sinhf, sinhl, sinl, sqrtf, sqrtl,
    tanf, tanhf, tanhl and tanl that are recognized in any mode since ISO C90 reserves
    these names for the purpose to which ISO C99 puts them. All these functions have
    corresponding versions prefixed with __builtin_. 
*/

/* DQ (4/12/2005): Required to compile the gnu version 3.4.3 cmath header file. Defined
//                 as functions instead of macros to avoid constant propagation issues.
*/
float       __builtin_acosf  (float __builtin__x);
long double __builtin_acosl  (long double __builtin__x);
float       __builtin_asinf  (float __builtin__x);
long double __builtin_asinl  (long double __builtin__x);
float       __builtin_atanf  (float __builtin__x);
long double __builtin_atanl  (long double __builtin__x);
float       __builtin_atan2f (float __builtin__x,float __builtin__y);
long double __builtin_atan2l (long double __builtin__x,long double __builtin__y);
float       __builtin_ceilf  (float __builtin__x);
long double __builtin_ceill  (long double __builtin__x);
float       __builtin_coshf  (float __builtin__x);
long double __builtin_coshl  (long double __builtin__x);
float       __builtin_floorf (float __builtin__x);
long double __builtin_floorl (long double __builtin__x);
float       __builtin_fmodf  (float __builtin__x,float __builtin__y);
long double __builtin_fmodl  (long double __builtin__x,long double __builtin__y);
float       __builtin_frexpf (float __builtin__x,int *__builtin__y);
long double __builtin_frexpl (long double __builtin__x,int *__builtin__y);
float       __builtin_ldexpf (float __builtin__x,float __builtin__y);
long double __builtin_ldexpl (long double __builtin__x,long double __builtin__y);
float       __builtin_log10f (float __builtin__x);
long double __builtin_log10l (long double __builtin__x);
float       __builtin_modff  (float __builtin__x,float *__builtin__y);
long double __builtin_modfl  (long double __builtin__x,long double *__builtin__y);
float       __builtin_powf   (float __builtin__x,float __builtin__y);
long double __builtin_powl   (long double __builtin__x,long double __builtin__y);
float       __builtin_sinhf  (float __builtin__x);
long double __builtin_sinhl  (long double __builtin__x);
float       __builtin_tanf   (float __builtin__x);
long double __builtin_tanl   (long double __builtin__x);
float       __builtin_tanhf  (float __builtin__x);
long double __builtin_tanhl  (long double __builtin__x);

/* DQ (5/15/2006): Suggested by Christian Biesinger (working with Markus Schordan) */
long double __builtin_powil  (long double __builtin__x, int __builtin__i);
double      __builtin_powi   (double __builtin__x, int __builtin__i);
float       __builtin_powif  (float __builtin__x, int __builtin__i);


/* The ISO C94 functions iswalnum, iswalpha, iswcntrl, iswdigit, iswgraph, iswlower,
    iswprint, iswpunct, iswspace, iswupper, iswxdigit, towlower and towupper are handled
    as built-in functions except in strict ISO C90 mode (-ansi or -std=c89). 
*/

/* The ISO C90 functions abort, abs, acos, asin, atan2, atan, calloc, ceil, cosh, cos,
    exit, exp, fabs, floor, fmod, fprintf, fputs, frexp, fscanf, isalnum, isalpha,
    iscntrl, isdigit, isgraph, islower, isprint, ispunct, isspace, isupper, isxdigit,
    tolower, toupper, labs, ldexp, log10, log, malloc, memcmp, memcpy, memset, modf, pow,
    printf, putchar, puts, scanf, sinh, sin, snprintf, sprintf, sqrt, sscanf, strcat,
    strchr, strcmp, strcpy, strcspn, strlen, strncat, strncmp, strncpy, strpbrk, strrchr,
    strspn, strstr, tanh, tan, vfprintf, vprintf and vsprintf are all recognized as
    built-in functions unless -fno-builtin is specified (or -fno-builtin-function is
    specified for an individual function). All of these functions have corresponding
    versions prefixed with __builtin_.
*/


/* DQ (7/29/2005): declarations for builtin functions used by GNU, but 
// already defined in EDG (it seems that we can provide declarations for 
// them explicitly).  These should be marked as compiler generated in 
// the AST.
*/
char *      __builtin_strchr (const char *__builtin__s, int __builtin__c);
char *      __builtin_strrchr(const char *__builtin__s, int __builtin__c);
char *      __builtin_strpbrk(const char *__builtin__s, const char *__builtin__accept);
char *      __builtin_strstr (const char *__builtin__haystack, const char *__builtin__needle);
float       __builtin_nansf  (const char *__builtin__x);
double      __builtin_nans   (const char *__builtin__x);
long double __builtin_nansl  (const char *__builtin__x);
double      __builtin_fabs   (double      __builtin__x);
float       __builtin_fabsf  (float       __builtin__x);
long double __builtin_fabsl  (long double __builtin__x);
float       __builtin_cosf   (float       __builtin__x);
long double __builtin_cosl   (long double __builtin__x);
float       __builtin_sinf   (float       __builtin__x);
long double __builtin_sinl   (long double __builtin__x);
float       __builtin_sqrtf  (float       __builtin__x);
long double __builtin_sqrtl  (long double __builtin__x);

/* GCC provides built-in versions of the ISO C99 floating point comparison macros that
    avoid raising exceptions for unordered operands. They have the same names as the
    standard macros ( isgreater, isgreaterequal, isless, islessequal, islessgreater, and
    isunordered) , with __builtin_ prefixed. We intend for a library implementor to be
    able to simply #define each standard macro to its built-in equivalent.

DQ (6/19/2007): These might required math.h to be included.
*/
# ifndef __builtin_isgreater
#  define __builtin_isgreater(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x > __y; }))
# endif

/* Return nonzero value if X is greater than or equal to Y.  */
# ifndef __builtin_isgreaterequal
#  define __builtin_isgreaterequal(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x >= __y; }))
# endif

/* Return nonzero value if X is less than Y.  */
# ifndef __builtin_isless
#  define __builtin_isless(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x < __y; }))
# endif

/* Return nonzero value if X is less than or equal to Y.  */
# ifndef __builtin_islessequal
#  define __builtin_islessequal(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x <= __y; }))
# endif

/* Return nonzero value if either X is less than Y or Y is less than X.  */
# ifndef __builtin_islessgreater
#  define __builtin_islessgreater(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && (__x < __y || __y < __x); }))
# endif

/* Return nonzero value if arguments are unordered.  */
# ifndef __builtin_isunordered
#  define __builtin_isunordered(u, v) \
  (__extension__							      \
   ({ __typeof__(u) __u = (u); __typeof__(v) __v = (v);			      \
      fpclassify (__u) == FP_NAN || fpclassify (__v) == FP_NAN; }))
# endif

/* int __builtin_isgreater(x,y) isgreater(x,y);
int __builtin_isgreaterequal(x,y) isgreaterequal(x,y);
int __builtin_isless(x,y) isless(x,y);
int __builtin_islessequal(x,y) islessequal(x,y);
int __builtin_islessgreater(x,y) islessgreater(x,y);
int __builtin_isunordered(x,y) isunordered(x,y);
*/

/* GNU also supports a few other types of builtin functions: */
void * __builtin_return_address (unsigned int level);
void * __builtin_frame_address (unsigned int level);

/* Additional builtin functions that take or return types as arguments
(described at http://gcc.gnu.org/onlinedocs/gcc-4.0.3/gcc/Other-Builtins.html).
There are more difficult to reproduce except as macros that define them away:

int __builtin_types_compatible_p (type1, type2);
type __builtin_choose_expr (const_exp, exp1, exp2);
int __builtin_constant_p (exp);

DQ (6/19/2007): The definitions below are not correct, but should be portable, 
they will only be a problem is the resulting code is unparsed directly from 
the AST.  The detection of the use of these macros in the near future will help 
make this safer.
*/

/* This is not a correct test, but it is a weak form of equivalence that is portable */
#define __builtin_types_compatible_p(T1,T2) (sizeof(T1)==sizeof(T2))
/* This is not correct, but it should be portable */
#define __builtin_choose_expr(exp,T1,T2) (T1)
/* This is not correct, but it should be portable, make it always return false for now */
#define __builtin_constant_p(exp) (0)


/* Additional builtin functions:
(also from http://gcc.gnu.org/onlinedocs/gcc-4.0.3/gcc/Other-Builtins.html):
 */

long        __builtin_expect (long __builtin__exp, long __builtin__c);
void        __builtin_prefetch (const void *__builtin__addr, ...);
double      __builtin_huge_val (void);
float       __builtin_huge_valf (void);
long double __builtin_huge_vall (void);
double      __builtin_inf (void);
float       __builtin_inff (void);
long double __builtin_infl (void);
double      __builtin_nan (const char *__builtin__str);
float       __builtin_nanf (const char *__builtin__str);
long double __builtin_nanl (const char *__builtin__str);
double      __builtin_nans (const char *__builtin__str);
float       __builtin_nansf (const char *__builtin__str);
long double __builtin_nansl (const char *__builtin__str);
/* DQ (6/19/2007): Commented out because in interferred with existing 
                   builtin function defined as int __builtin_ffs (int __builtin__x); in EDG.
int __builtin_ffs (unsigned int __builtin__x); */
int __builtin_clz (unsigned int __builtin__x);
int __builtin_ctz (unsigned int __builtin__x);
int __builtin_popcount (unsigned int __builtin__x);
int __builtin_parity (unsigned int __builtin__x);
int __builtin_ffsl (unsigned long __builtin__x);
int __builtin_clzl (unsigned long __builtin__x);
int __builtin_ctzl (unsigned long __builtin__x);
int __builtin_popcountl (unsigned long __builtin__x);
int __builtin_parityl (unsigned long __builtin__x);
int __builtin_ffsll (unsigned long long __builtin__x);
int __builtin_clzll (unsigned long long __builtin__x);
int __builtin_ctzll (unsigned long long __builtin__x);
int __builtin_popcountll (unsigned long long __builtin__x);
int __builtin_parityll (unsigned long long __builtin__x);
double      __builtin_powi (double __builtin__x, int __builtin__y);
float       __builtin_powif (float __builtin__x, int __builtin__y);
long double __builtin_powil (long double __builtin__x, int __builtin__y);


/* DQ (6/19/2007): I think these defines can be eliminated now in favor of the 
                   builtin function prototypes.
   DQ (8/20/2006): Let the builtin values be equal to the largest possible values.
   DQ (5/20/2006): These should be defined to be appropriate values or defined as 
   function prototypes as others are below.  These can't be functions because they 
   are used to initialize static const variables.

#define __builtin_huge_valf() __FLT_MAX__
#define __builtin_nanf(string) 0
#define __builtin_huge_val() __DBL_MAX__
#define __builtin_nan(string) 0
#define __builtin_huge_vall() __LDBL_MAX__
#define __builtin_nanl(string) 0
*/

/* DQ and Liao (7/11/2009) Added macros to define away new GNU C++ extension (not required for newer EDG 4.0 use */
// Only for GCC 4.3.0 and above
#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 3 || \
		     (__GNUC_MINOR__ == 3 && \
		      __GNUC_PATCHLEVEL__ >= 0)))

#define __has_nothrow_assign sizeof
#define __has_nothrow_copy sizeof
#define __has_nothrow_constructor sizeof
#define __has_trivial_assign sizeof
#define __has_trivial_copy  sizeof
#define __has_trivial_constructor sizeof
#define __has_trivial_destructor sizeof
#define __has_virtual_destructor sizeof
#define __is_abstract sizeof
//#define __is_base_of (base_type, derived_type)
#define __is_class sizeof
#define __is_empty sizeof
#define __is_enum sizeof
#define __is_pod sizeof 
#define __is_polymorphic sizeof
#define __is_union sizeof
//void* __builtin_memmove(void * target, const void * source, unsigned long long nBytes);
void* __builtin_memmove(void * target, const void * source, unsigned long nBytes);
void* __builtin_memchr(const  void * ptr, int value, unsigned long long num);
void* __builtin_memcpy (void * destination, const void * source, unsigned long long num );
int __builtin_memcmp ( const void * ptr1, const void * ptr2, unsigned long long num );
// changed it in edg 3.3/src/sys_predef.c instead since va_list is not declared here
//int __builtin_vsnprintf(char *str, unsigned long long size, const char *format, va_list ap);

#endif
/*
Target specific builtin functions are available at:
  http://gcc.gnu.org/onlinedocs/gcc-4.0.3/gcc/Target-Builtins.html
    * Alpha Built-in Functions
    * ARM Built-in Functions
    * Blackfin Built-in Functions
    * FR-V Built-in Functions
    * X86 Built-in Functions
    * MIPS Paired-Single Support
    * PowerPC AltiVec Built-in Functions
    * SPARC VIS Built-in Functions 
*/

/*
Required builtin function as supported by Intel (gnu builtin functions supported by Intel
C++ compiler).  I am not sure if we should include anything special specific to this, it
appears to be a subset of the more complete handling above.

__builtin_abs
__builtin_labs
__builtin_cos
__builtin_cosf
__builtin_fabs
__builtin_fabsf
__builtin_memcmp
__builtin_memcpy
__builtin_sin
__builtin_sinf
__builtin_sqrt
__builtin_sqrtf
__builtin_strcmp
__builtin_strlen
__builtin_strncmp
__builtin_abort
__builtin_prefetch
__builtin_constant_p
__builtin_printf
__builtin_fprintf
__builtin_fscanf
__builtin_scanf
__builtin_fputs
__builtin_memset
__builtin_strcat
__builtin_strcpy
__builtin_strncpy
__builtin_exit
__builtin_strchr
__builtin_strspn
__builtin_strcspn
__builtin_strstr
__builtin_strpbrk
__builtin_strrchr
__builtin_strncat
__builtin_alloca
__builtin_ffs
__builtin_index
__builtin_rindex
__builtin_bcmp
__builtin_bzero
__builtin_sinl
__builtin_cosl
__builtin_sqrtl
__builtin_fabsl
__builtin_frame_address (IA-32 only)
__builtin_return_address (IA-32 only)
*/

/* DQ (6/19/2007): For handling of offsetof macro we can't build a function 
prototype so EDG provides a mechanism to support this (from the basics.h 
header file). However, we can use the one defined in the GNU header files is 
we only handle the __offsetof__ macro (so define it way).  Then the builtin
function is just defined as being the offsetof macro.
*/
#define __offsetof__
/*
// DQ (2/9/2008): Don't define __builtin_offsetof in terms of offsetof since this
// causes a recursively defined marco on Fedora Core relase 4, though it works 
// fine on Red Hat Enterprise release 9. New definition taken from common
// implementation of offset, but modified to address specific case of where C++
// defines a operator&() which can be a problem for the more common definition.
// This definition is used to defin the __builtin_offsetof
// #define __builtin_offsetof(t,memb) offsetof(t,memb)
// #define __builtin_offsetof(t,memb) ((size_t)&(((t *)0)->memb))
*/
#ifndef __cplusplus
#define __builtin_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#else
/* The cast to "char &" below avoids problems with user-defined "operator &", which can appear in a POD type. */
#define __builtin_offsetof(TYPE, MEMBER)					\
  (__offsetof__ (reinterpret_cast <size_t>			\
                 (&reinterpret_cast <const volatile char &>	\
                  (static_cast<TYPE *> (0)->MEMBER))))
#endif

/* matching else for SKIP_ROSE_BUILTIN_DECLARATIONS */
#else

/* When compiling using -DSKIP_ROSE_BUILTIN_DECLARATIONS we need to have a variable
   defined in this faile so that we can locate the file in the AST and obtain the
   absolute path of this front-end specific header file.  This allows us to mark
   all IR nodes in the AST as being front-end specific if they originate from this
   header file. The following variable guarentees that a variable declaration from
   this file will exist when -DSKIP_ROSE_BUILTIN_DECLARATIONS is used.  The 
   -DSKIP_ROSE_BUILTIN_DECLARATIONS option is typically used to reduce the size of
   the AST and permit visualization of the whole AST using DOT (which can't layout
   a graph containing too many IR nodes).
 */
int __frontend_specific_variable_to_provide_header_file_path;

/* matching endif for SKIP_ROSE_BUILTIN_DECLARATIONS */
#endif

/* DQ (10/30/2005): Added to allow compilation of g++ 
complex header, but this is likely the wrong thing to do.
This allows us to compile C++ code that uses the complex header file
but it turns "__complex__ double x" into just "double x". All this
is because the "__complex__" keyword is not supported in EDG.  Not
too much of a problem except that it means that the member function:
    complex(_ComplexT __z) : _M_value(__z) { }
needs to be specified using the "explicit" keyword.   Note that
none of this is required in g++ or icc, just in our version of 
ROSE using the g++ header files (because __complex__ is not
defined in EDG and the only thing to map it to is "", I think).

The following solution does NOT work.  Replacing
    typedef __complex__ double _ComplexT;
with
    typedef complex<double> _ComplexT;
in the complex header file.

The following solution does work.  Replacing
    complex(_ComplexT __z) : _M_value(__z) { }
with
    explicit complex(_ComplexT __z) : _M_value(__z) { }
in the complex header file.  It is not a great solution
and it would have to be done in the configuration of ROSE
to the complex header file.  It would also fool any analysis
of the complex class into thinking that the internal type 
was just float or double for complex<float> or complex<double>.
 */

/* DQ (8/22/2006):
   EDG does not appear to support __complex__ as a keyword.
   Thus we have to define it to white space to permit codes using 
   __complex__ to be compiled.  This appears to work fine, but
   we are likely to confuse an analysis that depends upon recognizing 
   complex types.

   Note that _Complex is a C99 type, also fequently recognised by C 
   compilers, so for non C++ codes we can translate __complex__ to 
   _Complex
*/
/* #ifdef __cplusplus */
/* #if ROSE_CPP_MODE */
#if (ROSE_LANGUAGE_MODE == 1)
/* C++ case is different because the header files will define a complex type (see hdrs1/complex) */

  #define __complex__
  #define __real__ 
  #define __imag__

/* DQ (9/26/2006): Plum Hall uses this for C++ code, but I think that we have define it away */
  #define _Complex
#else
/* This works for both C and C99 modes */
  #define __complex__ _Complex
  #define __real__ 
  #define __imag__
#endif

/* gcc uses the C99 name _Complex_I in <complex.h>, but our EDG doesn't handle
   the GCC extension that they define it to. */
#define _Complex_I __I__
/* Disable inclusion of complex.h on Linux */
#define _COMPLEX_H
/* Disable inclusion of complex.h on Mac OS X */
#define __COMPLEX__

/* Defined this to avoid warnings (e.g. test2001_11.C) from 3.4.6 systems header files. */
#define __weakref__(NAME)

/* DQ (6/19/2007): I think we can comment this out now, since it is better defined above!
   DQ (1/31/2007): GNU modifier required to handle code using the offsetof macro in C++ g++ 3.4 and greater */
/* #define __offsetof__ */

#if 0
/* DQ (7/11/2009) Added macros to define away new GNU C++ extension (not required for newer EDG 4.0 use */
#define __has_nothrow_assign (type)
#define __has_nothrow_copy (type)
#define __has_nothrow_constructor (type)
#define __has_trivial_assign (type)
#define __has_trivial_copy (type)
#define __has_trivial_constructor (type)
#define __has_trivial_destructor (type)
#define __has_virtual_destructor (type)
#define __is_abstract (type)
#define __is_base_of (base_type, derived_type)
#define __is_class (type)
#define __is_empty (type)
#define __is_enum (type)
#define __is_pod (type)
#define __is_polymorphic (type)
#define __is_union (type)
#endif

/* 
   DQ (7/15/2009): Added support for MS Windows Code 
   It might be that this file in included too late when using WINE.
 */
#ifdef USE_ROSE_WINDOWS_ANALYSIS_SUPPORT
 #define __builtin_ms_va_list __builtin_va_list
 #define __ms_va_list va_list
#endif


#endif // !ROSE_USE_NEW_EDG_INTERFACE
