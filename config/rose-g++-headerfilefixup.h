
#ifndef _ROSE_GNU_HEADERFILEFIXUP_H
#define _ROSE_GNU_HEADERFILEFIXUP_H	1

#if 0
/* typedef __builtin_va_list __gnuc_va_list; */
char* __builtin_strchr(const char*,int);
char* __builtin_strpbrk(const char*, const char*);
char* __builtin_strrchr(const char*, int);
char* __builtin_strstr(const char*, const char*);

/* DQ (4/9/2005): This must be defined on the EDG commandline since this header file is not always read! */
/* bool __builtin_expect(bool,bool); */

char* __builtin_alloca(int);
double __builtin_fabs(double);
float __builtin_fabsf(float);
long double __builtin_fabsl(long double);
float __builtin_cosf(float);
long double __builtin_cosl(long double);
float __builtin_sinf(float);
long double __builtin_sinl(long double);
float __builtin_sqrtf(float);
long double __builtin_sqrtl(long double);
float __builtin_huge_valf();
// float __builtin_nanf(char*);
float __builtin_nansf(char*);
double __builtin_huge_val();
double __builtin_nan(char*);
double __builtin_nans(char*);
long double __builtin_huge_vall();
long double __builtin_nanl(char*);
long double __builtin_nansl(char*);

#define __typeof(type) void*
#define __asm__ 

// DQ (8/27/2004):
// I don't think we can handle the code in this header file so 
// just ignore it for now (if we can get away with that).
#define _BITS_ATOMICITY_H	1

// required elsewhere, but defined in atomicity.h 
// (which we turned off with by setting _BITS_ATOMICITY_H true)
typedef int _Atomic_word;

// DQ (8/27/2004):
// I don't think we can handle the code in this header file so 
// just ignore it for now (if we can get away with that).
#define _GLIBCPP_CONCEPT_CHECK 1

// Required for concept checking
// original definition in concept_check.h: #define __glibcpp_function_requires(...)
// #define __glibcpp_function_requires(type)
// #define __glibcpp_function_requires(type1,type2)
// #define __glibcpp_function_requires(type1,type2,type3)

// Turn off the code using this macro because it uses a GNU extension
// inorder to be compiled (in ROSE/g++_HEADERS/hdrs1/bits/stl_alloc.h).
#define _GLIBCPP_EXTERN_TEMPLATE 0

// used in locale_classes.h
void __atomic_add(_Atomic_word *,int);
int  __exchange_and_add(_Atomic_word *,int);

// used in limits
// #define __glibcpp_digits (type) (sizeof(type)*8)
// #define __CHAR_BIT__ unsigned(8)
#define __CHAR_BIT__ 7U
#define __SCHAR_MAX__ 127
#define __SHRT_MAX__ 127
#define __INT_MAX__ 2147483647
#define __LONG_MAX__ 2147483647
#define __LONG_LONG_MAX__ 2147483647
#define __FLT_MANT_DIG__ 8U
#define __FLT_DIG__ 8U
#define __FLT_RADIX__ 8U
#define __FLT_MIN_EXP__ 8U
#define __FLT_MIN_10_EXP__ 8U
#define __FLT_MAX_EXP__    0U
#define __FLT_MAX_10_EXP__ 0U
#define __builtin_huge_valf() 0
#define __builtin_nanf(string) 0
#define __FLT_DENORM_MIN__ 0
#define __FLT_MIN__ 0
#define __FLT_MAX__ 0
#define __FLT_EPSILON__ 0
#define __DBL_MANT_DIG__ 0
#define __DBL_DIG__ 0
#define __DBL_MIN_EXP__ 0
#define __DBL_MIN_10_EXP__ 0
#define __DBL_MAX_10_EXP__ 0
#define __DBL_MAX_EXP__ 0
#define __builtin_huge_val() 0
#define __builtin_nan(string) 0
#define __DBL_DENORM_MIN__ 0
#define __DBL_MIN__ 0
#define __DBL_MAX__ 0
#define __DBL_EPSILON__ 0
#define __LDBL_MANT_DIG__ 0
#define __LDBL_DIG__ 0
#define __LDBL_MIN_EXP__ 0
#define __LDBL_MIN_10_EXP__ 0
#define __LDBL_MAX_EXP__ 0
#define __LDBL_MAX_10_EXP__ 0
#define __builtin_huge_vall() 0
#define __builtin_nanl(string) 0
#define __LDBL_DENORM_MIN__ 0
#define __LDBL_MIN__ 0
#define __LDBL_MAX__ 0
#define __LDBL_EPSILON__ 0

/* __GNUC__ must be defined to be 3 (for 3.x) */
/* __GNUC_MINOR__ must be defined to 3 (for 3.3.x) and 4 (for 3.4.x) */

/* We need to have __flexarr defined as [1] (done incorrectly in /usr/include/sys/cdefs.h) */
#ifndef __flexarr
/* #warning "__flexarr is NOT defined" */
   #define __flexarr [1]
#else
/* #warning "__flexarr WAS already defined (was defined as [] but now redefined as [1])" */
   #undef __flexarr
   #define __flexarr [1]
#endif

/* DQ (6/4/2005): Need to compile g++ 3.4 header files used in test201_11.C
   This is not required now that we have emulated the correct __GNUC_PREREQ(maj, min)
   by setting __GNUC__ and __GNUC_MINOR__ correctly */
/* typedef long long int int64_t; */

/* DQ (6/4/2005): Need to compile g++ 3.4 header files used in test201_11.C 
   It appears sufficent to specify these here instead of in ROSE/config/compiler-defs.m4
   (the command line to EDG) 
 */
#define __FLT_HAS_INFINITY__ 1
#define __DBL_HAS_INFINITY__ 1
#define __LDBL_HAS_INFINITY__ 1
#define __FLT_HAS_QUIET_NAN__ 1
#define __DBL_HAS_QUIET_NAN__ 1
#define __LDBL_HAS_QUIET_NAN__ 1

#endif

/* _ROSE_GNU_HEADERFILEFIXUP_H */
#endif










