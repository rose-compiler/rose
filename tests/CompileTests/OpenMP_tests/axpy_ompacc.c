// Experimental test input for Accelerator directives
//  simplest scalar*vector operations
// Liao 1/15/2013
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

/*serial version */
void axpy(REAL* x, REAL* y, long n, REAL a) {
  int i;
  for (i = 0; i < n; ++i)
  {
    y[i] += a * x[i];
  }
}

/* compare two arrays and return percentage of difference */
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

void axpy_ompacc(REAL* x, REAL* y, int n, REAL a) {
  int i;
/* this one defines both the target device name and data environment to map to,
   I think here we need mechanism to tell the compiler the device type (could be multiple) so that compiler can generate the codes of different versions; 
   we also need to let the runtime know what the target device is so the runtime will chose the right function to call if the code are generated 
   #pragma omp target device (gpu0) map(x, y) 
*/
#pragma omp target device (0) map(tofrom: y[0:n]) map(to: x[0:n],a,n)
#pragma omp parallel for shared(x, y, n, a) private(i)
  for (i = 0; i < n; ++i)
    y[i] += a * x[i];
}

int main(int argc, char *argv[])
{
  int n;
  REAL *y_ompacc, *y, *x;
  REAL a = 123.456;

  n = VEC_LEN;

  y_ompacc = (REAL *) malloc(n * sizeof(REAL));
  y  = (REAL *) malloc(n * sizeof(REAL));
  x = (REAL *) malloc(n * sizeof(REAL));

  srand48(1<<12);
  init(x, n);
  init(y_ompacc, n);
  memcpy(y, y_ompacc, n*sizeof(REAL));

  axpy(x, y, n, a);

  /* openmp acc version */
  axpy_ompacc(x, y_ompacc, n, a);

  REAL checkresult = check(y_ompacc, y, n);
  printf("axpy(%d): checksum: %g\n", n, checkresult);
  assert (checkresult < 1.0e-10);

  free(y_ompacc);
  free(y);
  free(x);
  return 0;
}

