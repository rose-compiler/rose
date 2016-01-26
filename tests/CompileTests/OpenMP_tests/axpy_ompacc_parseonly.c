// Experimental test input for Accelerator directives
//  simplest scalar*vector operations
//  Testing extensions for multiple devices
//  This one has some exaggerated cases for testing parsing only
// Liao 2/2/2015
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#if 0
double time_stamp()
{
 struct timeval t;
 double time;
 gettimeofday(&t, NULL);
 time = t.tv_sec + 1.0e-6*t.tv_usec;
 return time;
}

#endif

/* in second */
#define read_timer() omp_get_wtime()
//#define read_timer() time_stamp()

/* change this to do saxpy or daxpy : single precision or double precision*/
#define REAL double
#define VEC_LEN 1024000 //use a fixed number for now
/* zero out the entire vector */
void zero(REAL *A, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        A[i] = 0.0;
    }
}

/* initialize a vector with random floating point numbers */
void init(REAL *A, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        A[i] = (double)drand48();
    }
}

REAL check(REAL*A, REAL*B, int n)
{
    int i;
    REAL sum = 0.0;
    for (i = 0; i < n; i++) {
        sum += A[i] - B[i];
    }
    return sum;
}

// reference CPU version
void axpy_omp(REAL* x, REAL* y, long n, REAL a) {
  int i;
//#pragma omp parallel for shared(x, y, n, a) private(i)
  for (i = 0; i < n; ++i)
  {
    y[i] += a * x[i];
  }
}

// GPU version
void axpy_ompacc(REAL* x, REAL* y, int n, REAL a) {
  int i;
//For testing parsing only, 3 policies are used for even 1-D arrays.  
#pragma omp target device (mpi) map(tofrom: y[0:n] dist_data(block, duplicate, cyclic(5)) ) map(to: x[0:n] dist_data(block(5), cyclic(3)),a,n)
#pragma omp parallel for shared(x, y, n, a) private(i)
  for (i = 0; i < n; ++i)
    y[i] += a * x[i];
}

int main(int argc, char *argv[])
{
  int n;
  REAL *y_omp, *y_ompacc, *x;
  REAL a = 123.456;

#pragma omp target device(mpi:all) begin
  n = VEC_LEN;
  y_omp = (REAL *) malloc(n * sizeof(REAL));
  y_ompacc = (REAL *) malloc(n * sizeof(REAL));
  x = (REAL *) malloc(n * sizeof(REAL));
#pragma omp target device(mpi:all) end  

#pragma omp target device(mpi:master) begin
  srand48(1<<12);
  init(x, n);
  init(y_ompacc, n);
  memcpy(y_ompacc, y_omp, n*sizeof(REAL));
#pragma omp target device(mpi:master) end

  int num_threads;
//  #pragma omp parallel shared (num_threads)
  {
    if (omp_get_thread_num() == 0)
      num_threads = omp_get_num_threads();
  }

 /* CPU threading version*/
 double omp_time = read_timer();
 axpy_omp(x, y_omp, n, a);
 omp_time = read_timer() - omp_time;

  /* openmp acc version */
  double ompacc_time = read_timer();
  axpy_ompacc(x, y_ompacc, n, a);
  ompacc_time = read_timer() - ompacc_time;

  printf("axpy(%d): checksum: %g; time(s):\tOMP(%d threads)\tOMPACC\n", n, check(y_omp, y_ompacc, n),num_threads);
  printf("\t\t\t\t\t\t%4f\t%4f\n", omp_time, ompacc_time);

  free(y_omp);
  free(y_ompacc);
  free(x);
  return 0;
}

