/*--------------------------------------------------------------------
  
  NAS Parallel Benchmarks 2.3 OpenMP C versions - CG

  This benchmark is an OpenMP C version of the NPB CG code.
  
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

  Authors: M. Yarrow
           C. Kuszmaul

  OpenMP C version: S. Satoh
  
--------------------------------------------------------------------*/

/*
c---------------------------------------------------------------------
c  Note: please observe that in the routine conj_grad three 
c  implementations of the sparse matrix-vector multiply have
c  been supplied.  The default matrix-vector multiply is not
c  loop unrolled.  The alternate implementations are unrolled
c  to a depth of 2 and unrolled to a depth of 8.  Please
c  experiment with these to find the fastest for your particular
c  architecture.  If reporting timing results, any of these three may
c  be used without penalty.
c---------------------------------------------------------------------
*/

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
#define NA      1400
#define NONZER  7
#define NITER   15
#define SHIFT   10.0
#define RCOND   1.0e-1
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'W'
/* CLASS = W */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define NA      7000
#define NONZER  8
#define NITER   15
#define SHIFT   12.0
#define RCOND   1.0e-1
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'A'
/* CLASS = A */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define NA      14000
#define NONZER  11
#define NITER   15
#define SHIFT   20.0
#define RCOND   1.0e-1
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'B'
/* CLASS = B */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define NA      75000
#define NONZER  13
#define NITER   75
#define SHIFT   60.0
#define RCOND   1.0e-1
#define CONVERTDOUBLE   FALSE
#endif

#if CLASS == 'C'
/* CLASS = C */
/*
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the classc of the NPB
c  in this directory. Do not modify it by hand.
*/
#define NA      150000
#define NONZER  15
#define NITER   75
#define SHIFT   110.0
#define RCOND   1.0e-1
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


#define	NZ	NA*(NONZER+1)*(NONZER+1)+NA*(NONZER+2)

/* global variables */

/* common /partit_size/ */
static int naa;
static int nzz;
static int firstrow;
static int lastrow;
static int firstcol;
static int lastcol;

/* common /main_int_mem/ */
static int colidx[NZ+1];	/* colidx[1:NZ] */
static int rowstr[NA+1+1];	/* rowstr[1:NA+1] */
static int iv[2*NA+1+1];	/* iv[1:2*NA+1] */
static int arow[NZ+1];		/* arow[1:NZ] */
static int acol[NZ+1];		/* acol[1:NZ] */

/* common /main_flt_mem/ */
static double v[NA+1+1];	/* v[1:NA+1] */
static double aelt[NZ+1];	/* aelt[1:NZ] */
static double a[NZ+1];		/* a[1:NZ] */
static double x[NA+2+1];	/* x[1:NA+2] */
static double z[NA+2+1];	/* z[1:NA+2] */
static double p[NA+2+1];	/* p[1:NA+2] */
static double q[NA+2+1];	/* q[1:NA+2] */
static double r[NA+2+1];	/* r[1:NA+2] */
static double w[NA+2+1];	/* w[1:NA+2] */

/* common /urando/ */
static double amult;
static double tran;

/* function declarations */
static void conj_grad (int colidx[], int rowstr[], double x[], double z[],
		       double a[], double p[], double q[], double r[],
		       double w[], double *rnorm);
static void makea(int n, int nz, double a[], int colidx[], int rowstr[],
		  int nonzer, int firstrow, int lastrow, int firstcol,
		  int lastcol, double rcond, int arow[], int acol[],
		  double aelt[], double v[], int iv[], double shift );
static void sparse(double a[], int colidx[], int rowstr[], int n,
		   int arow[], int acol[], double aelt[],
		   int firstrow, int lastrow,
		   double x[], boolean mark[], int nzloc[], int nnza);
static void sprnvc(int n, int nz, double v[], int iv[], int nzloc[],
		   int mark[]);
static int icnvrt(double x, int ipwr2);
static void vecset(int n, double v[], int iv[], int *nzv, int i, double val);

/*--------------------------------------------------------------------
      program cg
--------------------------------------------------------------------*/

int main(int argc, char **argv) {
    int	i, j, k, it;
    int nthreads = 1;
    double zeta;
    double rnorm;
    double norm_temp11;
    double norm_temp12;
    double t, mflops;
    char cclass;
    boolean verified;
    double zeta_verify_value, epsilon;

    firstrow = 1;
    lastrow  = NA;
    firstcol = 1;
    lastcol  = NA;

    if (NA == 1400 && NONZER == 7 && NITER == 15 && SHIFT == 10.0) {
	cclass = 'S';
	zeta_verify_value = 8.5971775078648;
    } else if (NA == 7000 && NONZER == 8 && NITER == 15 && SHIFT == 12.0) {
	cclass = 'W';
	zeta_verify_value = 10.362595087124;
    } else if (NA == 14000 && NONZER == 11 && NITER == 15 && SHIFT == 20.0) {
	cclass = 'A';
	zeta_verify_value = 17.130235054029;
    } else if (NA == 75000 && NONZER == 13 && NITER == 75 && SHIFT == 60.0) {
	cclass = 'B';
	zeta_verify_value = 22.712745482631;
    } else if (NA == 150000 && NONZER == 15 && NITER == 75 && SHIFT == 110.0) {
	cclass = 'C';
	zeta_verify_value = 28.973605592845;
    } else {
	cclass = 'U';
    }

    printf("\n\n NAS Parallel Benchmarks 2.3 OpenMP C version"
	   " - CG Benchmark\n");
    printf(" Size: %10d\n", NA);
    printf(" Iterations: %5d\n", NITER);

    naa = NA;
    nzz = NZ;

/*--------------------------------------------------------------------
c  Initialize random number generator
c-------------------------------------------------------------------*/
    tran    = 314159265.0;
    amult   = 1220703125.0;
    zeta    = randlc( &tran, amult );

/*--------------------------------------------------------------------
c  
c-------------------------------------------------------------------*/
    makea(naa, nzz, a, colidx, rowstr, NONZER,
	  firstrow, lastrow, firstcol, lastcol, 
	  RCOND, arow, acol, aelt, v, iv, SHIFT);
    
/*---------------------------------------------------------------------
c  Note: as a result of the above call to makea:
c        values of j used in indexing rowstr go from 1 --> lastrow-firstrow+1
c        values of colidx which are col indexes go from firstcol --> lastcol
c        So:
c        Shift the col index vals from actual (firstcol --> lastcol ) 
c        to local, i.e., (1 --> lastcol-firstcol+1)
c---------------------------------------------------------------------*/
#pragma omp parallel private(it,i,j,k)
{	
#pragma omp for nowait
    for (j = 1; j <= lastrow - firstrow + 1; j++) {
	for (k = rowstr[j]; k < rowstr[j+1]; k++) {
            colidx[k] = colidx[k] - firstcol + 1;
	}
    }

/*--------------------------------------------------------------------
c  set starting vector to (1, 1, .... 1)
c-------------------------------------------------------------------*/
#pragma omp for nowait
    for (i = 1; i <= NA+1; i++) {
	x[i] = 1.0;
    }
#pragma omp single
    zeta  = 0.0;

/*-------------------------------------------------------------------
c---->
c  Do one iteration untimed to init all code and data page tables
c---->                    (then reinit, start timing, to niter its)
c-------------------------------------------------------------------*/

    for (it = 1; it <= 1; it++) {

/*--------------------------------------------------------------------
c  The call to the conjugate gradient routine:
c-------------------------------------------------------------------*/
	conj_grad (colidx, rowstr, x, z, a, p, q, r, w, &rnorm);

/*--------------------------------------------------------------------
c  zeta = shift + 1/(x.z)
c  So, first: (x.z)
c  Also, find norm of z
c  So, first: (z.z)
c-------------------------------------------------------------------*/
#pragma omp single
{	
	norm_temp11 = 0.0;
	norm_temp12 = 0.0;
} /* end single */

#pragma omp for reduction(+:norm_temp11,norm_temp12)
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            norm_temp11 = norm_temp11 + x[j]*z[j];
            norm_temp12 = norm_temp12 + z[j]*z[j];
	}
#pragma omp single
	norm_temp12 = 1.0 / sqrt( norm_temp12 );

/*--------------------------------------------------------------------
c  Normalize z to obtain x
c-------------------------------------------------------------------*/
#pragma omp for
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            x[j] = norm_temp12*z[j];
	}
	
    } /* end of do one iteration untimed */

/*--------------------------------------------------------------------
c  set starting vector to (1, 1, .... 1)
c-------------------------------------------------------------------*/
#pragma omp for nowait
    for (i = 1; i <= NA+1; i++) {
         x[i] = 1.0;
    }
#pragma omp single    
    zeta  = 0.0;

} /* end parallel */

    timer_clear( 1 );
    timer_start( 1 );

/*--------------------------------------------------------------------
c---->
c  Main Iteration for inverse power method
c---->
c-------------------------------------------------------------------*/

#pragma omp parallel private(it,i,j,k)
{
    for (it = 1; it <= NITER; it++) {

/*--------------------------------------------------------------------
c  The call to the conjugate gradient routine:
c-------------------------------------------------------------------*/
	conj_grad(colidx, rowstr, x, z, a, p, q, r, w, &rnorm);

/*--------------------------------------------------------------------
c  zeta = shift + 1/(x.z)
c  So, first: (x.z)
c  Also, find norm of z
c  So, first: (z.z)
c-------------------------------------------------------------------*/
#pragma omp single
{	
	norm_temp11 = 0.0;
	norm_temp12 = 0.0;
} /* end single */

#pragma omp for reduction(+:norm_temp11,norm_temp12)
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            norm_temp11 = norm_temp11 + x[j]*z[j];
            norm_temp12 = norm_temp12 + z[j]*z[j];
	}

#pragma omp single
{	
	norm_temp12 = 1.0 / sqrt( norm_temp12 );

	zeta = SHIFT + 1.0 / norm_temp11;
} /* end single */

#pragma omp master
{
	if( it == 1 ) {
	  printf("   iteration           ||r||                 zeta\n");
	}
	printf("    %5d       %20.14e%20.13e\n", it, rnorm, zeta);
} /* end master */

/*--------------------------------------------------------------------
c  Normalize z to obtain x
c-------------------------------------------------------------------*/
#pragma omp for 
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            x[j] = norm_temp12*z[j];
	}
    } /* end of main iter inv pow meth */

#if defined(_OPENMP)
#pragma omp master
    nthreads = omp_get_num_threads();
#endif /* _OPENMP */
} /* end parallel */

    timer_stop( 1 );

/*--------------------------------------------------------------------
c  End of timed section
c-------------------------------------------------------------------*/

    t = timer_read( 1 );

    printf(" Benchmark completed\n");

    epsilon = 1.0e-10;
    if (cclass != 'U') {
	if (fabs(zeta - zeta_verify_value) <= epsilon) {
            verified = TRUE;
	    printf(" VERIFICATION SUCCESSFUL\n");
	    printf(" Zeta is    %20.12e\n", zeta);
	    printf(" Error is   %20.12e\n", zeta - zeta_verify_value);
	} else {
            verified = FALSE;
	    printf(" VERIFICATION FAILED\n");
	    printf(" Zeta                %20.12e\n", zeta);
	    printf(" The correct zeta is %20.12e\n", zeta_verify_value);
	}
    } else {
	verified = FALSE;
	printf(" Problem size unknown\n");
	printf(" NO VERIFICATION PERFORMED\n");
    }

    if ( t != 0.0 ) {
	mflops = (2.0*NITER*NA)
	    * (3.0+(NONZER*(NONZER+1)) + 25.0*(5.0+(NONZER*(NONZER+1))) + 3.0 )
	    / t / 1000000.0;
    } else {
	mflops = 0.0;
    }

    c_print_results("CG", cclass, NA, 0, 0, NITER, nthreads, t, 
		    mflops, "          floating point", 
		    verified, NPBVERSION, COMPILETIME,
		    CS1, CS2, CS3, CS4, CS5, CS6, CS7);
}

/*--------------------------------------------------------------------
c-------------------------------------------------------------------*/
static void conj_grad (
    int colidx[],	/* colidx[1:nzz] */
    int rowstr[],	/* rowstr[1:naa+1] */
    double x[],		/* x[*] */
    double z[],		/* z[*] */
    double a[],		/* a[1:nzz] */
    double p[],		/* p[*] */
    double q[],		/* q[*] */
    double r[],		/* r[*] */
    double w[],		/* w[*] */
    double *rnorm )
/*--------------------------------------------------------------------
c-------------------------------------------------------------------*/
    
/*---------------------------------------------------------------------
c  Floaging point arrays here are named as in NPB1 spec discussion of 
c  CG algorithm
c---------------------------------------------------------------------*/
{
    static double d, sum, rho, rho0, alpha, beta;
    int i, j, k;
    int cgit, cgitmax = 25;

#pragma omp single nowait
    rho = 0.0;
    
/*--------------------------------------------------------------------
c  Initialize the CG algorithm:
c-------------------------------------------------------------------*/
#pragma omp for nowait
    for (j = 1; j <= naa+1; j++) {
	q[j] = 0.0;
	z[j] = 0.0;
	r[j] = x[j];
	p[j] = r[j];
	w[j] = 0.0;
    }

/*--------------------------------------------------------------------
c  rho = r.r
c  Now, obtain the norm of r: First, sum squares of r elements locally...
c-------------------------------------------------------------------*/
#pragma omp for reduction(+:rho)
    for (j = 1; j <= lastcol-firstcol+1; j++) {
	rho = rho + x[j]*x[j];
    }

/*--------------------------------------------------------------------
c---->
c  The conj grad iteration loop
c---->
c-------------------------------------------------------------------*/
    for (cgit = 1; cgit <= cgitmax; cgit++) {
#pragma omp single nowait
{	
      rho0 = rho;
      d = 0.0;
      rho = 0.0;
} /* end single */
      
/*--------------------------------------------------------------------
c  q = A.p
c  The partition submatrix-vector multiply: use workspace w
c---------------------------------------------------------------------
C
C  NOTE: this version of the multiply is actually (slightly: maybe %5) 
C        faster on the sp2 on 16 nodes than is the unrolled-by-2 version 
C        below.   On the Cray t3d, the reverse is true, i.e., the 
C        unrolled-by-two version is some 10% faster.  
C        The unrolled-by-8 version below is significantly faster
C        on the Cray t3d - overall speed of code is 1.5 times faster.
*/

/* rolled version */      
#pragma omp for private(sum,k)
	for (j = 1; j <= lastrow-firstrow+1; j++) {
            sum = 0.0;
	    for (k = rowstr[j]; k < rowstr[j+1]; k++) {
		sum = sum + a[k]*p[colidx[k]];
	    }
            w[j] = sum;
	}
	
/* unrolled-by-two version
#pragma omp for private(i,k)
        for (j = 1; j <= lastrow-firstrow+1; j++) {
	    int iresidue;
	    double sum1, sum2;
	    i = rowstr[j]; 
            iresidue = (rowstr[j+1]-i) % 2;
            sum1 = 0.0;
            sum2 = 0.0;
            if (iresidue == 1) sum1 = sum1 + a[i]*p[colidx[i]];
	    for (k = i+iresidue; k <= rowstr[j+1]-2; k += 2) {
		sum1 = sum1 + a[k]   * p[colidx[k]];
		sum2 = sum2 + a[k+1] * p[colidx[k+1]];
	    }
            w[j] = sum1 + sum2;
        }
*/
/* unrolled-by-8 version
#pragma omp for private(i,k,sum)
        for (j = 1; j <= lastrow-firstrow+1; j++) {
	    int iresidue;
            i = rowstr[j]; 
            iresidue = (rowstr[j+1]-i) % 8;
            sum = 0.0;
            for (k = i; k <= i+iresidue-1; k++) {
                sum = sum +  a[k] * p[colidx[k]];
            }
            for (k = i+iresidue; k <= rowstr[j+1]-8; k += 8) {
                sum = sum + a[k  ] * p[colidx[k  ]]
                          + a[k+1] * p[colidx[k+1]]
                          + a[k+2] * p[colidx[k+2]]
                          + a[k+3] * p[colidx[k+3]]
                          + a[k+4] * p[colidx[k+4]]
                          + a[k+5] * p[colidx[k+5]]
                          + a[k+6] * p[colidx[k+6]]
                          + a[k+7] * p[colidx[k+7]];
            }
            w[j] = sum;
        }
*/
	
#pragma omp for
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            q[j] = w[j];
	}

/*--------------------------------------------------------------------
c  Clear w for reuse...
c-------------------------------------------------------------------*/
#pragma omp for	nowait
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            w[j] = 0.0;
	}

/*--------------------------------------------------------------------
c  Obtain p.q
c-------------------------------------------------------------------*/
#pragma omp for reduction(+:d)
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            d = d + p[j]*q[j];
	}

/*--------------------------------------------------------------------
c  Obtain alpha = rho / (p.q)
c-------------------------------------------------------------------*/
#pragma omp single	
	alpha = rho0 / d;

/*--------------------------------------------------------------------
c  Save a temporary of rho
c-------------------------------------------------------------------*/
	/*	rho0 = rho;*/

/*---------------------------------------------------------------------
c  Obtain z = z + alpha*p
c  and    r = r - alpha*q
c---------------------------------------------------------------------*/
#pragma omp for
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            z[j] = z[j] + alpha*p[j];
            r[j] = r[j] - alpha*q[j];
	}
            
/*---------------------------------------------------------------------
c  rho = r.r
c  Now, obtain the norm of r: First, sum squares of r elements locally...
c---------------------------------------------------------------------*/
#pragma omp for reduction(+:rho)	
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            rho = rho + r[j]*r[j];
	}

/*--------------------------------------------------------------------
c  Obtain beta:
c-------------------------------------------------------------------*/
#pragma omp single	
	beta = rho / rho0;

/*--------------------------------------------------------------------
c  p = r + beta*p
c-------------------------------------------------------------------*/
#pragma omp for
	for (j = 1; j <= lastcol-firstcol+1; j++) {
            p[j] = r[j] + beta*p[j];
	}
    } /* end of do cgit=1,cgitmax */

/*---------------------------------------------------------------------
c  Compute residual norm explicitly:  ||r|| = ||x - A.z||
c  First, form A.z
c  The partition submatrix-vector multiply
c---------------------------------------------------------------------*/
#pragma omp single nowait
    sum = 0.0;
    
#pragma omp for private(d, k)
    for (j = 1; j <= lastrow-firstrow+1; j++) {
	d = 0.0;
	for (k = rowstr[j]; k <= rowstr[j+1]-1; k++) {
            d = d + a[k]*z[colidx[k]];
	}
	w[j] = d;
    }

#pragma omp for
    for (j = 1; j <= lastcol-firstcol+1; j++) {
	r[j] = w[j];
    }

/*--------------------------------------------------------------------
c  At this point, r contains A.z
c-------------------------------------------------------------------*/
#pragma omp for reduction(+:sum) private(d)
    for (j = 1; j <= lastcol-firstcol+1; j++) {
	d = x[j] - r[j];
	sum = sum + d*d;
    }
    
#pragma omp single
  {
    (*rnorm) = sqrt(sum);
  } /* end single */
}

/*---------------------------------------------------------------------
c       generate the test problem for benchmark 6
c       makea generates a sparse matrix with a
c       prescribed sparsity distribution
c
c       parameter    type        usage
c
c       input
c
c       n            i           number of cols/rows of matrix
c       nz           i           nonzeros as declared array size
c       rcond        r*8         condition number
c       shift        r*8         main diagonal shift
c
c       output
c
c       a            r*8         array for nonzeros
c       colidx       i           col indices
c       rowstr       i           row pointers
c
c       workspace
c
c       iv, arow, acol i
c       v, aelt        r*8
c---------------------------------------------------------------------*/
static void makea(
    int n,
    int nz,
    double a[],		/* a[1:nz] */
    int colidx[],	/* colidx[1:nz] */
    int rowstr[],	/* rowstr[1:n+1] */
    int nonzer,
    int firstrow,
    int lastrow,
    int firstcol,
    int lastcol,
    double rcond,
    int arow[],		/* arow[1:nz] */
    int acol[],		/* acol[1:nz] */
    double aelt[],	/* aelt[1:nz] */
    double v[],		/* v[1:n+1] */
    int iv[],		/* iv[1:2*n+1] */
    double shift )
{
    int i, nnza, iouter, ivelt, ivelt1, irow, nzv;

/*--------------------------------------------------------------------
c      nonzer is approximately  (int(sqrt(nnza /n)));
c-------------------------------------------------------------------*/

    double size, ratio, scale;
    int jcol;

    size = 1.0;
    ratio = pow(rcond, (1.0 / (double)n));
    nnza = 0;

/*---------------------------------------------------------------------
c  Initialize colidx(n+1 .. 2n) to zero.
c  Used by sprnvc to mark nonzero positions
c---------------------------------------------------------------------*/
#pragma omp parallel for 
    for (i = 1; i <= n; i++) {
	colidx[n+i] = 0;
    }
    for (iouter = 1; iouter <= n; iouter++) {
	nzv = nonzer;
	sprnvc(n, nzv, v, iv, &(colidx[0]), &(colidx[n]));
	vecset(n, v, iv, &nzv, iouter, 0.5);
	for (ivelt = 1; ivelt <= nzv; ivelt++) {
	    jcol = iv[ivelt];
	    if (jcol >= firstcol && jcol <= lastcol) {
		scale = size * v[ivelt];
		for (ivelt1 = 1; ivelt1 <= nzv; ivelt1++) {
	            irow = iv[ivelt1];
                    if (irow >= firstrow && irow <= lastrow) {
			nnza = nnza + 1;
			if (nnza > nz) {
			    printf("Space for matrix elements exceeded in"
				   " makea\n");
			    printf("nnza, nzmax = %d, %d\n", nnza, nz);
			    printf("iouter = %d\n", iouter);
			    exit(1);
			}
			acol[nnza] = jcol;
			arow[nnza] = irow;
			aelt[nnza] = v[ivelt1] * scale;
		    }
		}
	    }
	}
	size = size * ratio;
    }

/*---------------------------------------------------------------------
c       ... add the identity * rcond to the generated matrix to bound
c           the smallest eigenvalue from below by rcond
c---------------------------------------------------------------------*/
    for (i = firstrow; i <= lastrow; i++) {
	if (i >= firstcol && i <= lastcol) {
	    iouter = n + i;
	    nnza = nnza + 1;
	    if (nnza > nz) {
		printf("Space for matrix elements exceeded in makea\n");
		printf("nnza, nzmax = %d, %d\n", nnza, nz);
		printf("iouter = %d\n", iouter);
		exit(1);
	    }
	    acol[nnza] = i;
	    arow[nnza] = i;
	    aelt[nnza] = rcond - shift;
	}
    }

/*---------------------------------------------------------------------
c       ... make the sparse matrix from list of elements with duplicates
c           (v and iv are used as  workspace)
c---------------------------------------------------------------------*/
    sparse(a, colidx, rowstr, n, arow, acol, aelt,
	   firstrow, lastrow, v, &(iv[0]), &(iv[n]), nnza);
}

/*---------------------------------------------------
c       generate a sparse matrix from a list of
c       [col, row, element] tri
c---------------------------------------------------*/
static void sparse(
    double a[],		/* a[1:*] */
    int colidx[],	/* colidx[1:*] */
    int rowstr[],	/* rowstr[1:*] */
    int n,
    int arow[],		/* arow[1:*] */
    int acol[],		/* acol[1:*] */
    double aelt[],	/* aelt[1:*] */
    int firstrow,
    int lastrow,
    double x[],		/* x[1:n] */
    boolean mark[],	/* mark[1:n] */
    int nzloc[],	/* nzloc[1:n] */
    int nnza)
/*---------------------------------------------------------------------
c       rows range from firstrow to lastrow
c       the rowstr pointers are defined for nrows = lastrow-firstrow+1 values
c---------------------------------------------------------------------*/
{
    int nrows;
    int i, j, jajp1, nza, k, nzrow;
    double xi;

/*--------------------------------------------------------------------
c    how many rows of result
c-------------------------------------------------------------------*/
    nrows = lastrow - firstrow + 1;

/*--------------------------------------------------------------------
c     ...count the number of triples in each row
c-------------------------------------------------------------------*/
#pragma omp parallel for     
    for (j = 1; j <= n; j++) {
	rowstr[j] = 0;
	mark[j] = FALSE;
    }
    rowstr[n+1] = 0;
    
    for (nza = 1; nza <= nnza; nza++) {
	j = (arow[nza] - firstrow + 1) + 1;
	rowstr[j] = rowstr[j] + 1;
    }

    rowstr[1] = 1;
    for (j = 2; j <= nrows+1; j++) {
	rowstr[j] = rowstr[j] + rowstr[j-1];
    }

/*---------------------------------------------------------------------
c     ... rowstr(j) now is the location of the first nonzero
c           of row j of a
c---------------------------------------------------------------------*/
    
/*--------------------------------------------------------------------
c     ... do a bucket sort of the triples on the row index
c-------------------------------------------------------------------*/
    for (nza = 1; nza <= nnza; nza++) {
	j = arow[nza] - firstrow + 1;
	k = rowstr[j];
	a[k] = aelt[nza];
	colidx[k] = acol[nza];
	rowstr[j] = rowstr[j] + 1;
    }

/*--------------------------------------------------------------------
c       ... rowstr(j) now points to the first element of row j+1
c-------------------------------------------------------------------*/
    for (j = nrows; j >= 1; j--) {
	rowstr[j+1] = rowstr[j];
    }
    rowstr[1] = 1;

/*--------------------------------------------------------------------
c       ... generate the actual output rows by adding elements
c-------------------------------------------------------------------*/
    nza = 0;
#pragma omp parallel for    
    for (i = 1; i <= n; i++) {
	x[i] = 0.0;
	mark[i] = FALSE;
    }

    jajp1 = rowstr[1];
    for (j = 1; j <= nrows; j++) {
	nzrow = 0;
	
/*--------------------------------------------------------------------
c          ...loop over the jth row of a
c-------------------------------------------------------------------*/
	for (k = jajp1; k < rowstr[j+1]; k++) {
            i = colidx[k];
            x[i] = x[i] + a[k];
            if ( mark[i] == FALSE && x[i] != 0.0) {
		mark[i] = TRUE;
		nzrow = nzrow + 1;
		nzloc[nzrow] = i;
	    }
	}

/*--------------------------------------------------------------------
c          ... extract the nonzeros of this row
c-------------------------------------------------------------------*/
	for (k = 1; k <= nzrow; k++) {
            i = nzloc[k];
            mark[i] = FALSE;
            xi = x[i];
            x[i] = 0.0;
            if (xi != 0.0) {
		nza = nza + 1;
		a[nza] = xi;
		colidx[nza] = i;
	    }
	}
	jajp1 = rowstr[j+1];
	rowstr[j+1] = nza + rowstr[1];
    }
}

/*---------------------------------------------------------------------
c       generate a sparse n-vector (v, iv)
c       having nzv nonzeros
c
c       mark(i) is set to 1 if position i is nonzero.
c       mark is all zero on entry and is reset to all zero before exit
c       this corrects a performance bug found by John G. Lewis, caused by
c       reinitialization of mark on every one of the n calls to sprnvc
---------------------------------------------------------------------*/
static void sprnvc(
    int n,
    int nz,
    double v[],		/* v[1:*] */
    int iv[],		/* iv[1:*] */
    int nzloc[],	/* nzloc[1:n] */
    int mark[] ) 	/* mark[1:n] */
{
    int nn1;
    int nzrow, nzv, ii, i;
    double vecelt, vecloc;

    nzv = 0;
    nzrow = 0;
    nn1 = 1;
    do {
	nn1 = 2 * nn1;
    } while (nn1 < n);

/*--------------------------------------------------------------------
c    nn1 is the smallest power of two not less than n
c-------------------------------------------------------------------*/

    while (nzv < nz) {
	vecelt = randlc(&tran, amult);

/*--------------------------------------------------------------------
c   generate an integer between 1 and n in a portable manner
c-------------------------------------------------------------------*/
	vecloc = randlc(&tran, amult);
	i = icnvrt(vecloc, nn1) + 1;
	if (i > n) continue;

/*--------------------------------------------------------------------
c  was this integer generated already?
c-------------------------------------------------------------------*/
	if (mark[i] == 0) {
	    mark[i] = 1;
	    nzrow = nzrow + 1;
	    nzloc[nzrow] = i;
	    nzv = nzv + 1;
	    v[nzv] = vecelt;
	    iv[nzv] = i;
	}
    }

    for (ii = 1; ii <= nzrow; ii++) {
	i = nzloc[ii];
	mark[i] = 0;
    }
}

/*---------------------------------------------------------------------
* scale a double precision number x in (0,1) by a power of 2 and chop it
*---------------------------------------------------------------------*/
static int icnvrt(double x, int ipwr2) {
    return ((int)(ipwr2 * x));
}

/*--------------------------------------------------------------------
c       set ith element of sparse vector (v, iv) with
c       nzv nonzeros to val
c-------------------------------------------------------------------*/
static void vecset(
    int n,
    double v[],	/* v[1:*] */
    int iv[],	/* iv[1:*] */
    int *nzv,
    int i,
    double val)
{
    int k;
    boolean set;

    set = FALSE;
    for (k = 1; k <= *nzv; k++) {
	if (iv[k] == i) {
            v[k] = val;
            set  = TRUE;
	}
    }
    if (set == FALSE) {
	*nzv = *nzv + 1;
	v[*nzv] = val;
	iv[*nzv] = i;
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

