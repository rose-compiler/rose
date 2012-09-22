/* ***********************************************************************
  This program is part of the
	OpenMP Source Code Repository

	http://www.pcg.ull.es/ompscr/
	e-mail: ompscr@etsii.ull.es

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License 
  (LICENSE file) along with this program; if not, write to
  the Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
  Boston, MA  02111-1307  USA
	
  FILE:              c_fft.c
  VERSION:           1.0
  DATE:              May 2004
  AUTHOR:            F. de Sande
  COMMENTS TO:       sande@csi.ull.es
  DESCRIPTION:       This program computes the Fast Fourier Transform
	                   on an input signal
  COMMENTS:	         The algorithm uses a divide and conquer strategy
	                   and the transform is computed as a combination of the
										 transforms of the even and odd terms of the original signal.
         						 The code requires nested Parallelism.
										 Function write_array() is provided only for debuging purposes.
										 (use a small size signal if you want to write it).
  REFERENCES:        James W. Cooley and John W. Tukey,
	                   An Algorithm for the Machine Calculation of Complex Fourier Series,
										 Mathematics of Computation, 1965, vol. 19, no. 90, pg 297-301
										 http://en.wikipedia.org/wiki/Cooley-Tukey_FFT_algorithm
  BASIC PRAGMAS:     parallel for
	USAGE:             ./c_fft.par 8192
  INPUT:             The size of the input signal
  OUTPUT:            The code tests the correctness of the result for the input
	FILE FORMATS:      -
	RESTRICTIONS:      The size of the input signal MUST be a power of 2
	REVISION HISTORY:  
**************************************************************************/

/* the expected result is that no  variable  should be reported as race condition, since it can make array A[i + n] only written by one thread  */


#include "OmpSCR.h"
#include <math.h>

#define KILO	  (1024)
#define DEFAULT_SIZE_IN_KB (64)
#define	NUM_ARGS	1
#define	NUM_TIMERS	1

typedef double doubleType;
typedef struct {
  doubleType re;
  doubleType im;
} Complex;

/* -----------------------------------------------------------------------
                          PROTOTYPES
 * ----------------------------------------------------------------------- */
void initialize(unsigned Size, Complex *a);
void write_array(unsigned Size, Complex *a);
int test_array(unsigned Size, Complex *a);
void FFT(Complex *A, Complex *a, Complex *W, unsigned N, unsigned stride, Complex *D);
void Roots(unsigned Size, Complex *W);
unsigned get_params(int argc, char *argv[]);

/* -----------------------------------------------------------------------
                          IMPLEMENTATION
 * ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
   Routine: initialize
   Description: Initialise a vector of complex numbers 
   Comment: all numbers have real part 1.0 and imaginary part 0.0
 * ----------------------------------------------------------------------- */
void initialize(unsigned Size, Complex *a) {
  unsigned i;

  for(i = 0; i < Size; i++) {
    a[i].re = 1.0;
    a[i].im = 0.0;
  }
}
/* -----------------------------------------------------------------------
   Routine: write_array
   Description: Display a vector of complex numbers 
 * ----------------------------------------------------------------------- */
void write_array(unsigned Size, Complex *a) {
  unsigned i;

  for(i = 0; i < Size; i++)
    printf("a[%2u] = [%.8lf,%.8lf]\n", i, a[i].re, a[i].im);
}
/* -----------------------------------------------------------------------
   Routine: test_array
   Description: Test is true if the complex vector is of the form 
     [(Size,0),(0,0),...,(0,0)]
 * ----------------------------------------------------------------------- */
int test_array(unsigned Size, Complex *a) {
  register unsigned i;
	unsigned OK = 1;

  if((a[0].re == Size) && (a[0].im == 0)) {
    for(i = 1; i < Size; i++)
      if (a[i].re != 0.0 || a[i].im != 0.0) {
        OK = 0;
        break;
      }
  }
  else OK = 0;
  return OK;
}
/* -----------------------------------------------------------------------
   Procedure: Roots
   Description: Computes roots of the Unary
   Parameters:
     unsigned Size, number of roots to compute
     Complex *W, vector containing the roots
 * ----------------------------------------------------------------------- */
void Roots(unsigned Size, Complex *W) {
  register unsigned i;
  double phi;
  Complex Omega;
  
  phi = 4 * atan(1.0) / (double)Size;                         /* PI/Size */
  Omega.re = cos(phi);
  Omega.im = sin(phi);
  W[0].re = 1.0;
  W[0].im = 0.0;
  for(i = 1; i < Size; i++) {
    W[i].re = W[i-1].re * Omega.re - W[i-1].im * Omega.im;
    W[i].im = W[i-1].re * Omega.im + W[i-1].im * Omega.re;
  }
}
/* -----------------------------------------------------------------------
   Procedure: FFT
   Description: Recursive (divide and conquer) Fast Fourier Transform 
   Parameters:
     Complex *A, transformed output signal
     Complex *a, input signal
     Complex *W, vector containing the roots
     unsigned N, number of elements in a
     unsigned stride, between consecutive elements in a to be considered
     Complex *D, auxiliar vector to do combination
 * ----------------------------------------------------------------------- */
void FFT(Complex *A, Complex *a, Complex *W, unsigned N,           
            unsigned stride, Complex *D) {
  Complex *B, *C;
  Complex Aux, *pW;
  unsigned n;
	int i;

  if (N == 1) {
    A[0].re = a[0].re;
    A[0].im = a[0].im;
  }
  else {
    /* Division stage without copying input data */
    n = (N >> 1);   /* N = N div 2 */

    /* Subproblems resolution stage */
#pragma omp parallel
   #pragma omp for
    for(i = 0; i <= 1; i++) {
      FFT(D + i * n, a + i * stride, W, n, stride << 1, A + i * n);
    }
    /* Combination stage */
    B = D;
    C = D + n;
#pragma omp parallel default(none) private( Aux, pW) shared(stride, n, A, B, C, W)
  #pragma omp for 
   for(i = 0; i <= n - 1; i++) {
      pW = W + i * stride;
      Aux.re = pW->re * C[i].re - pW->im * C[i].im;
      Aux.im = pW->re * C[i].im + pW->im * C[i].re;
      
      A[i].re = B[i].re + Aux.re;
      A[i].im = B[i].im + Aux.im;
      A[i+n].re = B[i].re - Aux.re;
      A[i+n].im = B[i].im - Aux.im;                                     
    }
  }
}
/* ----------------------------------------------------------------------- */
unsigned get_params(int argc, char *argv[]) {
	char usage_str[] = "<size_in_Kb>";
  unsigned sizeInKb;

  if (argc == 2)
    sizeInKb = atoi(argv[1]);
	else
		if (argc == 1)
			sizeInKb = DEFAULT_SIZE_IN_KB;
    else {
      printf("\nUse: %s %s\n", argv[0], usage_str);
      exit(-1);
    }
  printf("\nUse: %s %s\n", argv[0], usage_str);
  printf("Running with Size: %d K\n", sizeInKb);
	return sizeInKb;
}
/* ----------------------------------------------------------------------- */
int main(int argc, char *argv[]) {
  unsigned N;
  Complex *a, *A, *W, *D;
	int NUMTHREADS;
	char *PARAM_NAMES[NUM_ARGS] = {"Size of the input signal (in Kb)"};
	char *TIMERS_NAMES[NUM_TIMERS] = {"Total_time" };
	char *DEFAULT_VALUES[NUM_ARGS] = {"64"};
 	

 	NUMTHREADS = omp_get_max_threads();
	OSCR_init (NUMTHREADS, "Divide and Conquer Fast Fourier Transform.", "Use 'fft' <size (in K)>", NUM_ARGS, 
		PARAM_NAMES, DEFAULT_VALUES , NUM_TIMERS, NUM_TIMERS, TIMERS_NAMES, 
		argc, argv);

	N = KILO * OSCR_getarg_int(1);

  /* N = KILO * get_params(argc, argv); */
  
  /* Memory allocation */
  a = (Complex*)calloc(N, sizeof(Complex));
  A = (Complex*)calloc(N, sizeof(Complex));
  D = (Complex*)calloc(N, sizeof(Complex));
  W = (Complex*)calloc(N>>1, sizeof(Complex));
  if((a==NULL) || (A==NULL) || (D==NULL) || (W==NULL)) {
		printf("Not enough memory initializing arrays\n");
		exit(1);
	}
  initialize(N, a);                        /* Generate test input signal */
  /* write_array(N, a); */
  Roots(N >> 1, W);          /* Initialise the vector of imaginary roots */
  OSCR_timer_start(0);
  FFT(A, a, W, N, 1, D);
  OSCR_timer_stop(0);
  /* write_array(N, A); */

	/* Display results and time */
	printf("Test array: ");
	if (test_array(N, A))
		printf("Ok\n");
	else
		printf("Fails\n");
	OSCR_report(1, TIMERS_NAMES);
  free(W);
  free(D);
  free(A);
  free(a);

  return 0;
}

/*
 * vim:ts=2:sw=2:
 */
