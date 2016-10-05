// Experimental test input for Accelerator directives
//  simplest scalar*vector operations
//  Testing extensions for multiple devices
// Liao 7/1/2015
//AXPY multiple GPU version, using OpenMP 4.0 extended with data distribution
// vector = vector + vector * scalar
// The code will be transformed into axpy_ompacc3.c : using standard 4.0 directives only
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

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
        A[i] = (REAL)drand48();
    }
}

REAL check(REAL*A, REAL*B, int n)
{
    int i;
    REAL diffsum =0.0, sum = 0.0;
    for (i = 0; i < n; i++) {
        diffsum += fabs(A[i] - B[i]);
        sum += fabs(B[i]);
    }
    return diffsum/sum;
}

/* CPU version */
void axpy(REAL* x, REAL* y, long n, REAL a) {
  int i;
#pragma omp parallel for shared(x, y, n, a) private(i)
  for (i = 0; i < n; ++i)
  {
    y[i] += a * x[i];
  }
}

int main(int argc, char *argv[])
{
  int n,i;
  REAL *y_ref, *y_ompacc, *x;
  REAL a = 123.456f;

  n = VEC_LEN;
  if (argc >= 2)
    n = atoi(argv[1]);

  y_ref = (REAL *) malloc(n * sizeof(REAL));
  y_ompacc = (REAL *) malloc(n * sizeof(REAL));
  x = (REAL *) malloc(n * sizeof(REAL));

  srand48(1<<12);
  init(x, n);
  init(y_ref, n);
  memcpy(y_ompacc, y_ref, n*sizeof(REAL));

#if 0
//-------------------- begin of multi-gpu portion
  // Transformation point: obtain the number of devices to be used by default 
  int GPU_N = xomp_get_num_devices();
  printf("CUDA-capable device count: %i\n", GPU_N);

  // preparation for multiple GPUs
  // Transformation point: set first level thread count to be GPU count used
  omp_set_num_threads(GPU_N); 
#pragma omp parallel shared (GPU_N,x , y_ompacc, n) private(i)
  {
    int tid = omp_get_thread_num();
    xomp_set_default_device (tid);

    long size, offset;
    XOMP_static_even_divide (0, n, GPU_N, tid, &offset, &size);
    printf("thread %d working on GPU devices %d with size %d copying data from y_ompacc with offset %d\n",tid, tid, size,offset);
    int j;
#pragma omp target device (tid) map(tofrom: y_ompacc[offset:size]) map(to: x[offset:size],a,size, offset)
#pragma omp parallel for shared(size, a)  private(j)
    for (j = offset; j < offset+size; ++j)
    {
      y_ompacc[j] += a * x[j];
    }
  } 
//-------------------- end of multi-gpu portion
#else
  #pragma omp target device(*) map(tofrom: y_ompacc[0:n] dist_data(block)) map(to: x[0:n] dist_data(block),a,n)
  #pragma omp parallel for shared(x, y_ompacc, n, a) private(i)
  for (i = 0; i < n; ++i)
    y_ompacc[i] += a * x[i];

#endif

  int num_threads;
#pragma omp parallel shared (num_threads)
  {
    if (omp_get_thread_num() == 0)
      num_threads = omp_get_num_threads();
  }
  // serial version
  axpy(x, y_ref, n, a); 

  REAL checksum = check(y_ref, y_ompacc, n);
  printf("axpy(%d): checksum: %g\n", n, checksum);
  assert (checksum < 1.0e-10);

  free(y_ref);
  free(y_ompacc);
  free(x);
  return 0;
}
