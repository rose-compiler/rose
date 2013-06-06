/*
 * Copyright (c) 2004-2005 Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * MIT grants permission to use, copy, modify, and distribute this software and
 * its documentation for NON-COMMERCIAL purposes and without fee, provided that
 * this copyright notice appears in all copies.
 *
 * MIT provides this software "as is," without representations or warranties of
 * any kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose, and
 * noninfringement.  MIT shall not be liable for any damages arising from any
 * use of this software.
 *
 * Author: Alexandr Andoni (andoni@mit.edu), Piotr Indyk (indyk@mit.edu)
 */
 
#ifndef BASICDEFINITIONS_INCLUDED
#define BASICDEFINITIONS_INCLUDED

// PI-64: re-define the bits used to index points/buckets
//        also, try to make it work for both 32 and 64 bits
#define MACHINE_WORD_SIZE 64
#define N_BITS_FOR_BUCKET_LENGTH 10 /* PI: this was the actual value used in original lsh;
                                           should >3 */
#define N_BITS_PER_POINT_INDEX (MACHINE_WORD_SIZE - 2 - N_BITS_FOR_BUCKET_LENGTH)
//#define N_BITS_PER_POINT_INDEX 20
#define MAX_N_POINTS ((1U << N_BITS_PER_POINT_INDEX) - 1)

// PI: unsigned is mainly used for values (also used for indices in lsh...bug?)
//     signed is mainly used for indices and some algorithm-related parameters.
#define LongUns64T long long unsigned // PI: still 32 bits on 32-bit machines
#define Uns32T unsigned
#define Int32T int
#define BooleanT short //int
#define TRUE 1
#define FALSE 0
// PI-64: need to use #if to make it generic to 32/64 bits
#if MACHINE_WORD_SIZE == 64
# define Uns64T unsigned long int
# define Int64T long int
# define IntT Int64T
# define UnsT Uns64T
#else
# define Uns64T unsigned long long int
# define Int64T long long int
# define IntT Int32T
# define UnsT Uns32T
#endif
/* PI: used for 'R'=0.0 */
#define Pi_EPSILON 10e-6
#define Pi_PSEUDO_R 0.1

// TODO: this is the max available memory. Set for a particular
// machine at the moment. It is used in the function util.h/getAvailableMemory
#define DEFAULT_MEMORY_MAX_AVAILABLE 1000000000U

// The min value of 32-bits wide int. It should be actually
// (-2147483648), but the compiler will make it unsigned, and the
// problems (warnings) will begin.
#define MIN_INT32T (-2147483647)
// PI-64: -9223372036854775808+1
#define MIN_INT64T (-9223372036854775807)
#if MACHINE_WORD_SIZE == 64
# define MIN_INTT MIN_INT64T
#else
# define MIN_INTT MIN_INT32T
#endif

//#define DEBUG

// Attention: if this macro is defined, the program might be less
// portable.
#define DEBUG_MEM

// Attention: with this macro is defined, the program might be less
// portable. (However, if disabled, the self-tuning part will not
// work.)
#define DEBUG_TIMINGS

#define DEBUG_PROFILE_TIMING FALSE

// Note that if any of these flags is set to 'stdout', then when the
// main module outputs the computed parameters, the error/debug
// messages might interwine.
#define ERROR_OUTPUT stderr
#define DEBUG_OUTPUT stderr

// One of these three settings should be set externally (by the compiler).
//#define REAL_LONG_DOUBLE
//#define REAL_DOUBLE
//#define REAL_FLOAT

#ifdef REAL_LONG_DOUBLE
#define RealT long double
#define SQRT sqrtl
#define LOG logl
#define COS cosl
#define FLOOR_INT32(x) ((Int32T)(floorl(x)))
// PI-64
#define FLOOR_INT64(x) ((Int64T)(floorl(x)))
#define FLOOR(x) ((IntT)(floorl(x)))
#define CEIL(x) ((IntT)(ceill(x)))
#define POW(x, y) (powl(x, y))
#define FPRINTF_REAL(file, value) {fprintf(file, "%0.3Lf", value);}
#define FSCANF_REAL(file, value) {fscanf(file, "%Lf", value);}
#endif

#ifdef REAL_DOUBLE
#define RealT double
#define SQRT sqrt
#define LOG log
#define COS cos
#define FLOOR_INT32(x) ((Int32T)(floor(x)))
// PI-64
#define FLOOR_INT64(x) ((Int64T)(floor(x)))
#define FLOOR(x) ((IntT)(floor(x)))
#define CEIL(x) ((IntT)(ceil(x)))
#define POW(x, y) (pow(x, y))
#define FPRINTF_REAL(file, value) {fprintf(file, "%0.3lf", value);}
#define FSCANF_REAL(file, value) {fscanf(file, "%lf", value);}
#define EXP exp
#define ERF erf
#define ERFC erfc
#endif

#ifdef REAL_FLOAT
#define RealT float
#define SQRT sqrtf
#define LOG logf
#define COS cosf
#define FLOOR_INT32(x) ((Int32T)(floorf(x)))
// PI-64
#define FLOOR_INT64(x) ((Int64T)(floorf(x)))
#define FLOOR(x) ((IntT)(floorf(x)))
#define CEIL(x) ((IntT)(ceilf(x)))
#define POW(x, y) (powf(x, y))
#define FPRINTF_REAL(file, value) {fprintf(file, "%0.3f", value);}
#define FSCANF_REAL(file, value) {fscanf(file, "%f", value);}
#define EXP expf
#define ERF erf
#define ERFC erfc
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQR(a) ((a) * (a))

#define FAILIF(b) {if (b) {fprintf(ERROR_OUTPUT, "FAILIF triggered on line %d, file %s. Memory allocated: %ld\n", __LINE__, __FILE__, totalAllocatedMemory); abort();}}
#define FAILIFWR(b, s) {if (b) {fprintf(ERROR_OUTPUT, "FAILIF triggered on line %d, file %s. Memory allocated: %ld\nReason: %s\n", __LINE__, __FILE__, totalAllocatedMemory, s); abort();}}

#define ASSERT assert

// Critical ASSERT -- it is off when not debugging
#define CR_ASSERT ASSERT
#ifdef DEBUG
#define CR_ASSERTWR(b, reason) CR_ASSERT(b || !reason)
#else
#define NDEBUG
#define CR_ASSERTWR(b, reason)
#endif

#ifdef DEBUG 
#define DC {fprintf(DEBUG_OUTPUT, "Debug checkpoint. Line %d, file %s.\n", __LINE__, __FILE__);}
#define DPRINTF1(p1) {fprintf(DEBUG_OUTPUT, p1);}
#define DPRINTF(p1, p2) {fprintf(DEBUG_OUTPUT, p1, p2);}
#define DPRINTF3(p1, p2, p3) {fprintf(DEBUG_OUTPUT, p1, p2, p3);}
#define DPRINTF4(p1, p2, p3, p4) {fprintf(DEBUG_OUTPUT, p1, p2, p3, p4);}
#else
#define DC
#define DPRINTF1(p1)
#define DPRINTF(p1, p2)
#define DPRINTF3(p1, p2, p3)
#define DPRINTF4(p1, p2, p3, p4)
#endif

#define TimeVarT double

#ifdef DEBUG_TIMINGS
#define TIMEV_START(timeVar) { \
  if (timingOn) { \
    /*CR_ASSERTWR(timeVar >= 0, "timevar<0.");*/ \
    timeval _timev; \
    struct timezone _timez; \
    gettimeofday(&_timev, &_timez); \
    double timeInSecs = _timev.tv_sec + (double)_timev.tv_usec / 1000000.0; \
    timeVar -= timeInSecs; \
    /*int _b = (currentTime <= timeInSecs * 1.0000001);*/ \
    /*if (!_b) {*/ \
      /*printf("currentTime: %lf\n", currentTime);*/ \
      /*printf("timeInSecs: %lf\n", timeInSecs);*/ \
      /*printf("currentTime <= timeInSecs: %d\n", _b);*/ \
    /*}*/ \
    CR_ASSERTWR((currentTime <= timeInSecs * 1.0000001), "currentTime not increasing."); \
    CR_ASSERTWR(((currentTime = timeInSecs) >= 0), "timeInSecs < 0"); \
    /*CR_ASSERTWR(timeVar < 0, "timevar>=0");*/ \
  } \
}

#define TIMEV_END(timeVar) { \
  if (timingOn) { \
    /*CR_ASSERTWR(timeVar < 0, "timevar >=0");*/ \
    timeval _timev; \
    struct timezone _timez; \
    gettimeofday(&_timev, &_timez); \
    double timeInSecs = _timev.tv_sec + (double)_timev.tv_usec / 1000000.0; \
    timeVar += timeInSecs - timevSpeed; \
    if (timeVar < 0) { timeVar = 0;}; \
    /*int _b = (currentTime <= timeInSecs * 1.0000001);*/ \
    /*if (!_b) {*/ \
      /*printf("currentTime: %lf\n", currentTime);*/ \
      /*printf("timeInSecs: %lf\n", timeInSecs);*/ \
      /*printf("currentTime <= timeInSecs: %d\n", _b);*/ \
    /*}*/ \
    CR_ASSERTWR((currentTime <= timeInSecs * 1.0000001), "currentTime not increasing."); \
    /*CR_ASSERTWR(((currentTime = timeInSecs) >= 0), "timeInSecs < 0");*/ \
    /*CR_ASSERTWR(timeVar >= -0.0000001, "timevar <0")*/; \
  } \
}

#else
#define TIMEV_START(timeVar)
#define TIMEV_END(timeVar)
#endif


#endif
