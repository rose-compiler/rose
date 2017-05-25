/*--------------------------------------------------------------------
  
  NAS Parallel Benchmarks 2.3 OpenMP C versions - IS

  This benchmark is an OpenMP C version of the NPB IS code.
  
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

  Author: M. Yarrow

  OpenMP C version: S. Satoh
  
--------------------------------------------------------------------*/
//#include "npb-C.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
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
/*
   This file is generated automatically by the setparams utility.
   It sets the number of processors and the classc of the NPB
   in this directory. Do not modify it by hand.   */

#define COMPILETIME "07 Mar 2013"
#define NPBVERSION "2.3"
#define CC "identityTranslator "
#define CFLAGS "-rose:openmp:lowering "
#define CLINK "$(CC)"
#define CLINKFLAGS "-lm"
#define C_LIB "/export/tmp.liao6/workspace/thrifty/build64..."
#define C_INC "-I../common"

#include <stdlib.h>
#include <stdio.h>
#if defined(_OPENMP)
#include <omp.h>
#endif /* _OPENMP */


/*****************************************************************/
/* For serial IS, buckets are not really req'd to solve NPB1 IS  */
/* spec, but their use on some machines improves performance, on */
/* other machines the use of buckets compromises performance,    */
/* probably because it is extra computation which is not req'd.  */
/* (Note: Mechanism not understood, probably cache related)      */
/* Example:  SP2-66MhzWN:  50% speedup with buckets              */
/* Example:  SGI Indy5000: 50% slowdown with buckets             */
/* Example:  SGI O2000:   400% slowdown with buckets (Wow!)      */
/*****************************************************************/
/* #define USE_BUCKETS  */
/* buckets are not used in the OpenMP C version */


#define CLASS 'A'

/******************/
/* default values */
/******************/
#ifndef CLASS
#define CLASS 'A'
#endif


/*************/
/*  CLASS S  */
/*************/
#if CLASS == 'S'
#define  TOTAL_KEYS_LOG_2    16
#define  MAX_KEY_LOG_2       11
#define  NUM_BUCKETS_LOG_2   9
#endif


/*************/
/*  CLASS W  */
/*************/
#if CLASS == 'W'
#define  TOTAL_KEYS_LOG_2    20
#define  MAX_KEY_LOG_2       16
#define  NUM_BUCKETS_LOG_2   10
#endif

/*************/
/*  CLASS A  */
/*************/
#if CLASS == 'A'
#define  TOTAL_KEYS_LOG_2    23
#define  MAX_KEY_LOG_2       19
#define  NUM_BUCKETS_LOG_2   10
#endif


/*************/
/*  CLASS B  */
/*************/
#if CLASS == 'B'
#define  TOTAL_KEYS_LOG_2    25
#define  MAX_KEY_LOG_2       21
#define  NUM_BUCKETS_LOG_2   10
#endif


/*************/
/*  CLASS C  */
/*************/
#if CLASS == 'C'
#define  TOTAL_KEYS_LOG_2    27
#define  MAX_KEY_LOG_2       23
#define  NUM_BUCKETS_LOG_2   10
#endif


#define  TOTAL_KEYS          (1 << TOTAL_KEYS_LOG_2)
#define  MAX_KEY             (1 << MAX_KEY_LOG_2)
#define  NUM_BUCKETS         (1 << NUM_BUCKETS_LOG_2)
#define  NUM_KEYS            TOTAL_KEYS
#define  SIZE_OF_BUFFERS     NUM_KEYS  
                                           

#define  MAX_ITERATIONS      10
#define  TEST_ARRAY_SIZE     5


/*************************************/
/* Typedef: if necessary, change the */
/* size of int here by changing the  */
/* int type to, say, long            */
/*************************************/
typedef  int  INT_TYPE;


/********************/
/* Some global info */
/********************/
INT_TYPE *key_buff_ptr_global;         /* used by full_verify to get */
                                       /* copies of rank info        */

int      passed_verification;
                                 

/************************************/
/* These are the three main arrays. */
/* See SIZE_OF_BUFFERS def above    */
/************************************/
INT_TYPE key_array[SIZE_OF_BUFFERS],    
         key_buff1[SIZE_OF_BUFFERS],    
         key_buff2[SIZE_OF_BUFFERS],
         partial_verify_vals[TEST_ARRAY_SIZE];

#ifdef USE_BUCKETS
INT_TYPE bucket_size[NUM_BUCKETS],                    
         bucket_ptrs[NUM_BUCKETS];
#endif


/**********************/
/* Partial verif info */
/**********************/
INT_TYPE test_index_array[TEST_ARRAY_SIZE],
         test_rank_array[TEST_ARRAY_SIZE],

         S_test_index_array[TEST_ARRAY_SIZE] = 
                             {48427,17148,23627,62548,4431},
         S_test_rank_array[TEST_ARRAY_SIZE] = 
                             {0,18,346,64917,65463},

         W_test_index_array[TEST_ARRAY_SIZE] = 
                             {357773,934767,875723,898999,404505},
         W_test_rank_array[TEST_ARRAY_SIZE] = 
                             {1249,11698,1039987,1043896,1048018},

         A_test_index_array[TEST_ARRAY_SIZE] = 
                             {2112377,662041,5336171,3642833,4250760},
         A_test_rank_array[TEST_ARRAY_SIZE] = 
                             {104,17523,123928,8288932,8388264},

         B_test_index_array[TEST_ARRAY_SIZE] = 
                             {41869,812306,5102857,18232239,26860214},
         B_test_rank_array[TEST_ARRAY_SIZE] = 
                             {33422937,10244,59149,33135281,99}, 

         C_test_index_array[TEST_ARRAY_SIZE] = 
                             {44172927,72999161,74326391,129606274,21736814},
         C_test_rank_array[TEST_ARRAY_SIZE] = 
                             {61147,882988,266290,133997595,133525895};



/***********************/
/* function prototypes */
/***********************/
static double	randlc2( double *X, double *A );

void full_verify( void );

/*
 *    FUNCTION RANDLC (X, A)
 *
 *  This routine returns a uniform pseudorandom double precision number in the
 *  range (0, 1) by using the linear congruential generator
 *
 *  x_{k+1} = a x_k  (mod 2^46)
 *
 *  where 0 < x_k < 2^46 and 0 < a < 2^46.  This scheme generates 2^44 numbers
 *  before repeating.  The argument A is the same as 'a' in the above formula,
 *  and X is the same as x_0.  A and X must be odd double precision integers
 *  in the range (1, 2^46).  The returned value RANDLC is normalized to be
 *  between 0 and 1, i.e. RANDLC = 2^(-46) * x_1.  X is updated to contain
 *  the new seed x_1, so that subsequent calls to RANDLC using the same
 *  arguments will generate a continuous sequence.
 *
 *  This routine should produce the same results on any computer with at least
 *  48 mantissa bits in double precision floating point data.  On Cray systems,
 *  double precision should be disabled.
 *
 *  David H. Bailey     October 26, 1990
 *
 *     IMPLICIT DOUBLE PRECISION (A-H, O-Z)
 *     SAVE KS, R23, R46, T23, T46
 *     DATA KS/0/
 *
 *  If this is the first call to RANDLC, compute R23 = 2 ^ -23, R46 = 2 ^ -46,
 *  T23 = 2 ^ 23, and T46 = 2 ^ 46.  These are computed in loops, rather than
 *  by merely using the ** operator, in order to insure that the results are
 *  exact on all systems.  This code assumes that 0.5D0 is represented exactly.
 */


/*****************************************************************/
/*************           R  A  N  D  L  C             ************/
/*************                                        ************/
/*************    portable random number generator    ************/
/*****************************************************************/

static double	randlc2(double *X, double *A)
{
      static int        KS=0;
      static double	R23, R46, T23, T46;
      double		T1, T2, T3, T4;
      double		A1;
      double		A2;
      double		X1;
      double		X2;
      double		Z;
      int     		i, j;

      if (KS == 0) 
      {
        R23 = 1.0;
        R46 = 1.0;
        T23 = 1.0;
        T46 = 1.0;
    
        for (i=1; i<=23; i++)
        {
          R23 = 0.50 * R23;
          T23 = 2.0 * T23;
        }
        for (i=1; i<=46; i++)
        {
          R46 = 0.50 * R46;
          T46 = 2.0 * T46;
        }
        KS = 1;
      }

/*  Break A into two parts such that A = 2^23 * A1 + A2 and set X = N.  */

      T1 = R23 * *A;
      j  = T1;
      A1 = j;
      A2 = *A - T23 * A1;

/*  Break X into two parts such that X = 2^23 * X1 + X2, compute
    Z = A1 * X2 + A2 * X1  (mod 2^23), and then
    X = 2^23 * Z + A2 * X2  (mod 2^46).                            */

      T1 = R23 * *X;
      j  = T1;
      X1 = j;
      X2 = *X - T23 * X1;
      T1 = A1 * X2 + A2 * X1;
      
      j  = R23 * T1;
      T2 = j;
      Z = T1 - T23 * T2;
      T3 = T23 * Z + A2 * X2;
      j  = R46 * T3;
      T4 = j;
      *X = T3 - T46 * T4;
      return(R46 * *X);
} 




/*****************************************************************/
/*************      C  R  E  A  T  E  _  S  E  Q      ************/
/*****************************************************************/

void	create_seq( double seed, double a )
{
	double x;
	int    i, j, k;

        k = MAX_KEY/4;

	for (i=0; i<NUM_KEYS; i++)
	{
	    x = randlc2(&seed, &a);
	    x += randlc2(&seed, &a);
    	    x += randlc2(&seed, &a);
	    x += randlc2(&seed, &a);  

            key_array[i] = k*x;
	}
}




/*****************************************************************/
/*************    F  U  L  L  _  V  E  R  I  F  Y     ************/
/*****************************************************************/


void full_verify()
{
    INT_TYPE    i, j;
    INT_TYPE    k;
    INT_TYPE    m, unique_keys;


    
/*  Now, finally, sort the keys:  */
    for( i=0; i<NUM_KEYS; i++ )
        key_array[--key_buff_ptr_global[key_buff2[i]]] = key_buff2[i];


/*  Confirm keys correctly sorted: count incorrectly sorted keys, if any */

    j = 0;
    for( i=1; i<NUM_KEYS; i++ )
        if( key_array[i-1] > key_array[i] )
            j++;


    if( j != 0 )
    {
        printf( "Full_verify: number of keys out of sort: %d\n",
                j );
    }
    else
        passed_verification++;
           

}




/*****************************************************************/
/*************             R  A  N  K             ****************/
/*****************************************************************/

void rank( int iteration )
{

    INT_TYPE    i, j, k;
    INT_TYPE    l, m;

    INT_TYPE    shift = MAX_KEY_LOG_2 - NUM_BUCKETS_LOG_2;
    INT_TYPE    key;
    INT_TYPE    min_key_val, max_key_val;

    INT_TYPE	prv_buff1[MAX_KEY];

#pragma omp master
  {
    key_array[iteration] = iteration;
    key_array[iteration+MAX_ITERATIONS] = MAX_KEY - iteration;

/*  Determine where the partial verify test keys are, load into  */
/*  top of array bucket_size                                     */
    for( i=0; i<TEST_ARRAY_SIZE; i++ )
        partial_verify_vals[i] = key_array[test_index_array[i]];

/*  Clear the work array */
    for( i=0; i<MAX_KEY; i++ )
        key_buff1[i] = 0;
  }
#pragma omp barrier  

  for (i=0; i<MAX_KEY; i++)
      prv_buff1[i] = 0;

/*  Copy keys into work array; keys in key_array will be reused each iter. */
#pragma omp for nowait
    for( i=0; i<NUM_KEYS; i++ ) {
        key_buff2[i] = key_array[i];

/*  Ranking of all keys occurs in this section:                 */

/*  In this section, the keys themselves are used as their 
    own indexes to determine how many of each there are: their
    individual population                                       */

        prv_buff1[key_buff2[i]]++;  /* Now they have individual key   */
    }
                                       /* population                     */
    for( i=0; i<MAX_KEY-1; i++ )   
        prv_buff1[i+1] += prv_buff1[i];  


#pragma omp critical
    {
	for( i=0; i<MAX_KEY; i++ )
	    key_buff1[i] += prv_buff1[i];
    }

/*  To obtain ranks of each key, successively add the individual key
    population, not forgetting to add m, the total of lesser keys,
    to the first key population                                          */

#pragma omp barrier    
#pragma omp master
  {
    
/* This is the partial verify test section */
/* Observe that test_rank_array vals are   */
/* shifted differently for different cases */
    for( i=0; i<TEST_ARRAY_SIZE; i++ )
    {                                             
        k = partial_verify_vals[i];          /* test vals were put here */
        if( 0 <= k  &&  k <= NUM_KEYS-1 )
            switch( CLASS )
            {
                case 'S':
                    if( i <= 2 )
                    {
                        if( key_buff1[k-1] != test_rank_array[i]+iteration )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
                    }
                    else
                    {
                        if( key_buff1[k-1] != test_rank_array[i]-iteration )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
                    }
                    break;
                case 'W':
                    if( i < 2 )
                    {
                        if( key_buff1[k-1] != 
                                          test_rank_array[i]+(iteration-2) )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
                    }
                    else
                    {
                        if( key_buff1[k-1] != test_rank_array[i]-iteration )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
                    }
                    break;
                case 'A':
                    if( i <= 2 )
        	    {
                        if( key_buff1[k-1] != 
                                          test_rank_array[i]+(iteration-1) )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
        	    }
                    else
                    {
                        if( key_buff1[k-1] != 
                                          test_rank_array[i]-(iteration-1) )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
                    }
                    break;
                case 'B':
                    if( i == 1 || i == 2 || i == 4 )
        	    {
                        if( key_buff1[k-1] != test_rank_array[i]+iteration )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
        	    }
                    else
                    {
                        if( key_buff1[k-1] != test_rank_array[i]-iteration )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
                    }
                    break;
                case 'C':
                    if( i <= 2 )
        	    {
                        if( key_buff1[k-1] != test_rank_array[i]+iteration )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
        	    }
                    else
                    {
                        if( key_buff1[k-1] != test_rank_array[i]-iteration )
                        {
                            printf( "Failed partial verification: "
                                  "iteration %d, test key %d\n", 
                                   iteration, i );
                        }
                        else
                            passed_verification++;
                    }
                    break;
            }        
    }




/*  Make copies of rank info for use by full_verify: these variables
    in rank are local; making them global slows down the code, probably
    since they cannot be made register by compiler                        */

    if( iteration == MAX_ITERATIONS ) 
        key_buff_ptr_global = key_buff1;

  } /* end master */
}      


/*****************************************************************/
/*************             M  A  I  N             ****************/
/*****************************************************************/

int main(int argc, char** argv )
{

    int             i, iteration, itemp;
    int		    nthreads = 1;
    double          timecounter, maxtime;



/*  Initialize the verification arrays if a valid class */
    for( i=0; i<TEST_ARRAY_SIZE; i++ )
        switch( CLASS )
        {
            case 'S':
                test_index_array[i] = S_test_index_array[i];
                test_rank_array[i]  = S_test_rank_array[i];
                break;
            case 'A':
                test_index_array[i] = A_test_index_array[i];
                test_rank_array[i]  = A_test_rank_array[i];
                break;
            case 'W':
                test_index_array[i] = W_test_index_array[i];
                test_rank_array[i]  = W_test_rank_array[i];
                break;
            case 'B':
                test_index_array[i] = B_test_index_array[i];
                test_rank_array[i]  = B_test_rank_array[i];
                break;
            case 'C':
                test_index_array[i] = C_test_index_array[i];
                test_rank_array[i]  = C_test_rank_array[i];
                break;
        };

        

/*  Printout initial NPB info */
    printf( "\n\n NAS Parallel Benchmarks 2.3 OpenMP C version"
	    " - IS Benchmark\n\n" );
    printf( " Size:  %d  (class %c)\n", TOTAL_KEYS, CLASS );
    printf( " Iterations:   %d\n", MAX_ITERATIONS );

/*  Initialize timer  */             
    timer_clear( 0 );

/*  Generate random number sequence and subsequent keys on all procs */
    create_seq( 314159265.00,                    /* Random number gen seed */
                1220703125.00 );                 /* Random number gen mult */


/*  Do one interation for free (i.e., untimed) to guarantee initialization of  
    all data and code pages and respective tables */
#pragma omp parallel    
    rank( 1 );  

/*  Start verification counter */
    passed_verification = 0;

    if( CLASS != 'S' ) printf( "\n   iteration\n" );

/*  Start timer  */             
    timer_start( 0 );


/*  This is the main iteration */
    
#pragma omp parallel private(iteration)    
    for( iteration=1; iteration<=MAX_ITERATIONS; iteration++ )
    {
#pragma omp master	
        if( CLASS != 'S' ) printf( "        %d\n", iteration );
	
        rank( iteration );
	
#if defined(_OPENMP)	
#pragma omp master
	nthreads = omp_get_num_threads();
#endif /* _OPENMP */	
    }

/*  End of timing, obtain maximum time of all processors */
    timer_stop( 0 );
    timecounter = timer_read( 0 );


/*  This tests that keys are in sequence: sorting of last ranked key seq
    occurs here, but is an untimed operation                             */
    full_verify();



/*  The final printout  */
    if( passed_verification != 5*MAX_ITERATIONS + 1 )
        passed_verification = 0;
    c_print_results( "IS",
                     CLASS,
                     TOTAL_KEYS,
                     0,
                     0,
                     MAX_ITERATIONS,
		     nthreads,
                     timecounter,
                     ((double) (MAX_ITERATIONS*TOTAL_KEYS))
                                                  /timecounter/1000000.,
                     "keys ranked", 
                     passed_verification,
                     NPBVERSION,
                     COMPILETIME,
                     CC,
                     CLINK,
                     C_LIB,
                     C_INC,
                     CFLAGS,
                     CLINKFLAGS,
		     "randlc2");


  return 0;
         /**************************/
}        /*  E N D  P R O G R A M  */
         /**************************/

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
//  gettimeofday(&tv, (void *)0);
  gettimeofday(&tv, (struct timezone *)0);
  if (sec < 0) sec = tv.tv_sec;
  *t = (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

