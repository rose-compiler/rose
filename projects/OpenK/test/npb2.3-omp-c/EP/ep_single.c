/*--------------------------------------------------------------------
  
  NAS Parallel Benchmarks 2.3 OpenMP C versions - EP

  This benchmark is an OpenMP C version of the NPB EP code.
  
  The OpenMP C versions are developed by RWCP and derived from the serial
  Fortran versions in "NPB 2.3-serial" developed by NAS.

  Permission to use, copy, distribute and modify this software for any
  purpose with or without fee is hereby granted.
  This software is provided "as is" without express or implied warranty.
  
  Send comments on the OpenMP C versions to pdp-openmp@rwcp.or.jp

  Information on OpenMP activities at RWCP is available at:

           http://pdplab.trc.rwcp.or.jp/pdperf/Omni/
  
  Information on NAS Parallel Benchmarks 2.3 is available at:
  
           http://www.nas.nasa.gov/NAS/NPB/

--------------------------------------------------------------------*/
/*--------------------------------------------------------------------

  Author: P. O. Frederickson 
          D. H. Bailey
          A. C. Woo

  OpenMP C version: S. Satoh
  
--------------------------------------------------------------------*/

//#include "npb-C.h"
/*
  NAS Parallel Benchmarks 2.3 OpenMP C Versions
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if defined(_OPENMP)
#include <omp.h>
#endif /* _OPENMP */

typedef int boolean;
typedef struct { double real; double imag; } dcomplex;

#define TRUE    1
#define FALSE   0

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define pow2(a) ((a)*(a))

#define get_real(c) c.real
#define get_imag(c) c.imag
#define cadd(c,a,b) (c.real = a.real + b.real, c.imag = a.imag + b.imag)
#define csub(c,a,b) (c.real = a.real - b.real, c.imag = a.imag - b.imag)
#define cmul(c,a,b) (c.real = a.real * b.real - a.imag * b.imag, \
                     c.imag = a.real * b.imag + a.imag * b.real)
#define crmul(c,a,b) (c.real = a.real * b, c.imag = a.imag * b)

extern double randlc(double *, double);
extern void vranlc(int, double *, double, double *);
extern void timer_clear(int);
extern void timer_start(int);
extern void timer_stop(int);
extern double timer_read(int);

extern void c_print_results(char *name, char cclass, int n1, int n2,
                            int n3, int niter, int nthreads, double t,
                            double mops, char *optype, int passed_verification,
                            char *npbversion, char *compiletime, char *cc,
                            char *clink, char *c_lib, char *c_inc,
                            char *cflags, char *clinkflags, char *rand);

//#include "npbparams.h"

/******************/
/* default values */
/******************/
#ifndef CLASS
#define CLASS 'B'
#endif

#if CLASS == 'S'
/* CLASS = S */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define CLASS    'S'
#define M       24
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'W'
/* CLASS = W */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define CLASS    'W'
#define M       25
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'A'
/* CLASS = A */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define CLASS    'A'
#define M       28
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'B'
/* CLASS = B */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define CLASS    'B'
#define M       30
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'C'
/* CLASS = C */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define CLASS    'C'
#define M       32
#define CONVERTDOUBLE   FALSE
#endif


#define COMPILETIME "28 Oct 2014"
#define NPBVERSION "2.3"
#define CS1 "gcc"
#define CS2 "$(CC)"
#define CS3 "(none)"
#define CS4 "-I../common"
#define CS5 "-fopenmp -O2"
#define CS6 "-lm -fopenmp"
#define CS7 "randdp"


/* parameters */
#define	MK		16
#define	MM		(M - MK)
#define	NN		(1 << MM)
#define	NK		(1 << MK)
#define	NQ		10
#define EPSILON		1.0e-8
#define	A		1220703125.0
#define	S		271828183.0
#define	TIMERS_ENABLED	FALSE

/* global variables */
/* common /storage/ */
static double x[2*NK];
#pragma omp threadprivate(x)
static double q[NQ];

/*--------------------------------------------------------------------
      program EMBAR
c-------------------------------------------------------------------*/
/*
c   This is the serial version of the APP Benchmark 1,
c   the "embarassingly parallel" benchmark.
c
c   M is the Log_2 of the number of complex pairs of uniform (0, 1) random
c   numbers.  MK is the Log_2 of the size of each batch of uniform random
c   numbers.  MK can be set for convenience on a given system, since it does
c   not affect the results.
*/
int main(int argc, char **argv) {

    double Mops, t1, t2, t3, t4, x1, x2, sx, sy, tm, an, tt, gc;
    double dum[3] = { 1.0, 1.0, 1.0 };
    int np, ierr, node, no_nodes, i, ik, kk, l, k, nit, ierrcode,
	no_large_nodes, np_add, k_offset, j;
    int nthreads = 1;
    boolean verified;
    char size[13+1];	/* character*13 */

/*
c   Because the size of the problem is too large to store in a 32-bit
c   integer for some classes, we put it into a string (for printing).
c   Have to strip off the decimal point put in there by the floating
c   point print statement (internal file)
*/

    printf("\n\n NAS Parallel Benchmarks 2.3 OpenMP C version"
	   " - EP Benchmark\n");
    sprintf(size, "%12.0f", pow(2.0, M+1));
    for (j = 13; j >= 1; j--) {
	if (size[j] == '.') size[j] = ' ';
    }
    printf(" Number of random numbers generated: %13s\n", size);

    verified = FALSE;

/*
c   Compute the number of "batches" of random number pairs generated 
c   per processor. Adjust if the number of processors does not evenly 
c   divide the total number
*/
    np = NN;

/*
c   Call the random number generator functions and initialize
c   the x-array to reduce the effects of paging on the timings.
c   Also, call all mathematical functions that are used. Make
c   sure these initializations cannot be eliminated as dead code.
*/
    vranlc(0, &(dum[0]), dum[1], &(dum[2]));
    dum[0] = randlc(&(dum[1]), dum[2]);
    for (i = 0; i < 2*NK; i++) x[i] = -1.0e99;
    Mops = log(sqrt(fabs(max(1.0, 1.0))));

    timer_clear(1);
    timer_clear(2);
    timer_clear(3);
    timer_start(1);

    vranlc(0, &t1, A, x);

/*   Compute AN = A ^ (2 * NK) (mod 2^46). */

    t1 = A;

    for ( i = 1; i <= MK+1; i++) {
	t2 = randlc(&t1, t1);
    }

    an = t1;
    tt = S;
    gc = 0.0;
    sx = 0.0;
    sy = 0.0;

    for ( i = 0; i <= NQ - 1; i++) {
	q[i] = 0.0;
    }
      
/*
c   Each instance of this loop may be performed independently. We compute
c   the k offsets separately to take into account the fact that some nodes
c   have more numbers to generate than others
*/
    k_offset = -1;

#pragma omp parallel copyin(x)
{
    double t1, t2, t3, t4, x1, x2;
    int kk, i, ik, l;
    double qq[NQ];		/* private copy of q[0:NQ-1] */

    for (i = 0; i < NQ; i++) qq[i] = 0.0;

#pragma omp for reduction(+:sx,sy) schedule(static)  
    for (k = 1; k <= np; k++) {
	kk = k_offset + k;
	t1 = S;
	t2 = an;

/*      Find starting seed t1 for this kk. */

	for (i = 1; i <= 100; i++) {
            ik = kk / 2;
            if (2 * ik != kk) t3 = randlc(&t1, t2);
            if (ik == 0) break;
            t3 = randlc(&t2, t2);
            kk = ik;
	}

/*      Compute uniform pseudorandom numbers. */

	if (TIMERS_ENABLED == TRUE) timer_start(3);
	vranlc(2*NK, &t1, A, x-1);
	if (TIMERS_ENABLED == TRUE) timer_stop(3);

/*
c       Compute Gaussian deviates by acceptance-rejection method and 
c       tally counts in concentric square annuli.  This loop is not 
c       vectorizable.
*/
	if (TIMERS_ENABLED == TRUE) timer_start(2);

	for ( i = 0; i < NK; i++) {
            x1 = 2.0 * x[2*i] - 1.0;
            x2 = 2.0 * x[2*i+1] - 1.0;
            t1 = pow2(x1) + pow2(x2);
            if (t1 <= 1.0) {
		t2 = sqrt(-2.0 * log(t1) / t1);
		t3 = (x1 * t2);				/* Xi */
		t4 = (x2 * t2);				/* Yi */
		l = max(fabs(t3), fabs(t4));
		qq[l] += 1.0;				/* counts */
		sx = sx + t3;				/* sum of Xi */
		sy = sy + t4;				/* sum of Yi */
            }
	}
	if (TIMERS_ENABLED == TRUE) timer_stop(2);
    }
#pragma omp critical
    {
      for (i = 0; i <= NQ - 1; i++) q[i] += qq[i];
    }
#if defined(_OPENMP)
#pragma omp master
    nthreads = omp_get_num_threads();
#endif /* _OPENMP */    
} /* end of parallel region */    

    for (i = 0; i <= NQ-1; i++) {
        gc = gc + q[i];
    }

    timer_stop(1);
    tm = timer_read(1);

    nit = 0;
    if (M == 24) {
	if((fabs((sx- (-3.247834652034740e3))/sx) <= EPSILON) &&
	   (fabs((sy- (-6.958407078382297e3))/sy) <= EPSILON)) {
	    verified = TRUE;
	}
    } else if (M == 25) {
	if ((fabs((sx- (-2.863319731645753e3))/sx) <= EPSILON) &&
	    (fabs((sy- (-6.320053679109499e3))/sy) <= EPSILON)) {
	    verified = TRUE;
	}
    } else if (M == 28) {
      {
	if ((fabs((sx- (-4.295875165629892e3))/sx) <= EPSILON) &&
	    (fabs((sy- (-1.580732573678431e4))/sy) <= EPSILON)) {
	    verified = TRUE;
	}
         printf("Debug: 231, sx is:%f, sy is:%f\n",sx,sy);
      }
    } else if (M == 30) {
	if ((fabs((sx- (4.033815542441498e4))/sx) <= EPSILON) &&
	    (fabs((sy- (-2.660669192809235e4))/sy) <= EPSILON)) {
	    verified = TRUE;
	}
    } else if (M == 32) {
	if ((fabs((sx- (4.764367927995374e4))/sx) <= EPSILON) &&
	    (fabs((sy- (-8.084072988043731e4))/sy) <= EPSILON)) {
	    verified = TRUE;
	}
    }

    Mops = pow(2.0, M+1)/tm/1000000.0;

    printf("EP Benchmark Results: \n"
	   "CPU Time = %10.4f\n"
	   "N = 2^%5d\n"
	   "No. Gaussian Pairs = %15.0f\n"
	   "Sums = %25.15e %25.15e\n"
	   "Counts:\n",
	   tm, M, gc, sx, sy);
    for (i = 0; i  <= NQ-1; i++) {
	printf("%3d %15.0f\n", i, q[i]);
    }
	  
    c_print_results("EP", CLASS, M+1, 0, 0, nit, nthreads,
		  tm, Mops, 	
		  "Random numbers generated",
		  verified, NPBVERSION, COMPILETIME,
		  CS1, CS2, CS3, CS4, CS5, CS6, CS7);

    if (TIMERS_ENABLED == TRUE) {
	printf("Total time:     %f", timer_read(1));
	printf("Gaussian pairs: %f", timer_read(2));
	printf("Random numbers: %f", timer_read(3));
    }
}




/* cat ./common/c_print_results.c */
/*****************************************************************/
/******     C  _  P  R  I  N  T  _  R  E  S  U  L  T  S     ******/
/*****************************************************************/
void c_print_results( char   *name,
                      char   cclass,
                      int    n1, 
                      int    n2,
                      int    n3,
                      int    niter,
                      int    nthreads,
                      double t,
                      double mops,
                      char   *optype,
                      int    passed_verification,
                      char   *npbversion,
                      char   *compiletime,
                      char   *cc,
                      char   *clink,
                      char   *c_lib,
                      char   *c_inc,
                      char   *cflags,
                      char   *clinkflags,
                      char   *rand)
{
    char *evalue="1000";

    printf( "\n\n %s Benchmark Completed\n", name ); 

    printf( " Class           =                        %c\n", cclass );

    if( n2 == 0 && n3 == 0 )
        printf( " Size            =             %12d\n", n1 );   /* as in IS */
    else
        printf( " Size            =              %3dx%3dx%3d\n", n1,n2,n3 );

    printf( " Iterations      =             %12d\n", niter );
    
    printf( " Threads         =             %12d\n", nthreads );
 
    printf( " Time in seconds =             %12.2f\n", t );

    printf( " Mop/s total     =             %12.2f\n", mops );

    printf( " Operation type  = %24s\n", optype);

    if( passed_verification )
        printf( " Verification    =               SUCCESSFUL\n" );
    else
        printf( " Verification    =             UNSUCCESSFUL\n" );

    printf( " Version         =             %12s\n", npbversion );

    printf( " Compile date    =             %12s\n", compiletime );

    printf( "\n Compile options:\n" );

    printf( "    CC           = %s\n", cc );

    printf( "    CLINK        = %s\n", clink );

    printf( "    C_LIB        = %s\n", c_lib );

    printf( "    C_INC        = %s\n", c_inc );

    printf( "    CFLAGS       = %s\n", cflags );

    printf( "    CLINKFLAGS   = %s\n", clinkflags );

    printf( "    RAND         = %s\n", rand );
#ifdef SMP
    evalue = getenv("MP_SET_NUMTHREADS");
    printf( "   MULTICPUS = %s\n", evalue );
#endif

/*    printf( "\n\n" );
    printf( " Please send the results of this run to:\n\n" );
    printf( " NPB Development Team\n" );
    printf( " Internet: npb@nas.nasa.gov\n \n" );
    printf( " If email is not available, send this to:\n\n" );
    printf( " MS T27A-1\n" );
    printf( " NASA Ames Research Center\n" );
    printf( " Moffett Field, CA  94035-1000\n\n" );
    printf( " Fax: 415-604-3957\n\n" );*/
}


/*
cat ./common/c_timers.c
*/
/*
#include "wtime.h"
#if defined(IBM)
#define wtime wtime
#elif defined(CRAY)
#define wtime WTIME
#else
#define wtime wtime_
#endif

*/
/*  Prototype  */
void wtime( double * );


/*****************************************************************/
/******         E  L  A  P  S  E  D  _  T  I  M  E          ******/
/*****************************************************************/
double elapsed_time( void )
{
    double t;

    wtime( &t );
    return( t );
}


double start[64], elapsed[64];

/*****************************************************************/
/******            T  I  M  E  R  _  C  L  E  A  R          ******/
/*****************************************************************/
void timer_clear( int n )
{
    elapsed[n] = 0.0;
}


/*****************************************************************/
/******            T  I  M  E  R  _  S  T  A  R  T          ******/
/*****************************************************************/
void timer_start( int n )
{
    start[n] = elapsed_time();
}


/*****************************************************************/
/******            T  I  M  E  R  _  S  T  O  P             ******/
/*****************************************************************/
void timer_stop( int n )
{
    double t, now;

    now = elapsed_time();
    t = now - start[n];
    elapsed[n] += t;

}


/*****************************************************************/
/******            T  I  M  E  R  _  R  E  A  D             ******/
/*****************************************************************/
double timer_read( int n )
{
    return( elapsed[n] );
}


void wtime(double *t)
{
  static int sec = -1;
  struct timeval tv;
  gettimeofday(&tv, (void *)0);
//  gettimeofday(&tv, (struct timezone *)0);
  if (sec < 0) sec = tv.tv_sec;
  *t = (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

// common/c_randdp.c
/*
*/
#if defined(USE_POW)
#define r23 pow(0.5, 23.0)
#define r46 (r23*r23)
#define t23 pow(2.0, 23.0)
#define t46 (t23*t23)
#else
#define r23 (0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5*0.5)
#define r46 (r23*r23)
#define t23 (2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0*2.0)
#define t46 (t23*t23)
#endif

/*c---------------------------------------------------------------------
c---------------------------------------------------------------------*/

double randlc (double *x, double a) {

/*c---------------------------------------------------------------------
c---------------------------------------------------------------------*/

/*c---------------------------------------------------------------------
c
c   This routine returns a uniform pseudorandom double precision number in the
c   range (0, 1) by using the linear congruential generator
c
c   x_{k+1} = a x_k  (mod 2^46)
c
c   where 0 < x_k < 2^46 and 0 < a < 2^46.  This scheme generates 2^44 numbers
c   before repeating.  The argument A is the same as 'a' in the above formula,
c   and X is the same as x_0.  A and X must be odd double precision integers
c   in the range (1, 2^46).  The returned value RANDLC is normalized to be
c   between 0 and 1, i.e. RANDLC = 2^(-46) * x_1.  X is updated to contain
c   the new seed x_1, so that subsequent calls to RANDLC using the same
c   arguments will generate a continuous sequence.
c
c   This routine should produce the same results on any computer with at least
c   48 mantissa bits in double precision floating point data.  On 64 bit
c   systems, double precision should be disabled.
c
c   David H. Bailey     October 26, 1990
c
c---------------------------------------------------------------------*/

    double t1,t2,t3,t4,a1,a2,x1,x2,z;

/*c---------------------------------------------------------------------
c   Break A into two parts such that A = 2^23 * A1 + A2.
c---------------------------------------------------------------------*/
    t1 = r23 * a;
    a1 = (int)t1;
    a2 = a - t23 * a1;
/*c---------------------------------------------------------------------
c   Break X into two parts such that X = 2^23 * X1 + X2, compute
c   Z = A1 * X2 + A2 * X1  (mod 2^23), and then
c   X = 2^23 * Z + A2 * X2  (mod 2^46).
c---------------------------------------------------------------------*/
    t1 = r23 * (*x);
    x1 = (int)t1;
    x2 = (*x) - t23 * x1;
    t1 = a1 * x2 + a2 * x1;
    t2 = (int)(r23 * t1);
    z = t1 - t23 * t2;
    t3 = t23 * z + a2 * x2;
    t4 = (int)(r46 * t3);
    (*x) = t3 - t46 * t4;

    return (r46 * (*x));
}


/*c---------------------------------------------------------------------
c---------------------------------------------------------------------*/

void vranlc (int n, double *x_seed, double a, double* y) {
/* void vranlc (int n, double *x_seed, double a, double y[]) { */

/*c---------------------------------------------------------------------
c---------------------------------------------------------------------*/

/*c---------------------------------------------------------------------
c
c   This routine generates N uniform pseudorandom double precision numbers in
c   the range (0, 1) by using the linear congruential generator
c
c   x_{k+1} = a x_k  (mod 2^46)
c
c   where 0 < x_k < 2^46 and 0 < a < 2^46.  This scheme generates 2^44 numbers
c   before repeating.  The argument A is the same as 'a' in the above formula,
c   and X is the same as x_0.  A and X must be odd double precision integers
c   in the range (1, 2^46).  The N results are placed in Y and are normalized
c   to be between 0 and 1.  X is updated to contain the new seed, so that
c   subsequent calls to VRANLC using the same arguments will generate a
c   continuous sequence.  If N is zero, only initialization is performed, and
c   the variables X, A and Y are ignored.
c
c   This routine is the standard version designed for scalar or RISC systems.
c   However, it should produce the same results on any single processor
c   computer with at least 48 mantissa bits in double precision floating point
c   data.  On 64 bit systems, double precision should be disabled.
c
c---------------------------------------------------------------------*/

    int i;
    double x,t1,t2,t3,t4,a1,a2,x1,x2,z;

/*c---------------------------------------------------------------------
c   Break A into two parts such that A = 2^23 * A1 + A2.
c---------------------------------------------------------------------*/
    t1 = r23 * a;
    a1 = (int)t1;
    a2 = a - t23 * a1;
    x = *x_seed;

/*c---------------------------------------------------------------------
c   Generate N results.   This loop is not vectorizable.
c---------------------------------------------------------------------*/
    for (i = 1; i <= n; i++) {

/*c---------------------------------------------------------------------
c   Break X into two parts such that X = 2^23 * X1 + X2, compute
c   Z = A1 * X2 + A2 * X1  (mod 2^23), and then
c   X = 2^23 * Z + A2 * X2  (mod 2^46).
c---------------------------------------------------------------------*/
        t1 = r23 * x;
        x1 = (int)t1;
        x2 = x - t23 * x1;
        t1 = a1 * x2 + a2 * x1;
        t2 = (int)(r23 * t1);
        z = t1 - t23 * t2;
        t3 = t23 * z + a2 * x2;
        t4 = (int)(r46 * t3);
        x = t3 - t46 * t4;
        y[i] = r46 * x;
    }
    *x_seed = x;
}

