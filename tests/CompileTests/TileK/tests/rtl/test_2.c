
#ifndef ORIGINAL
#define ORIGINAL 0
#endif
#if ORIGINAL == 0
#  include "RTL/Host/klt-user.h"
#  include "KLT/RTL/kernel.h"
#  include "KLT/RTL/loop.h"
#  include "KLT/RTL/tile.h"
#  include "KLT/RTL/data.h"
#endif

#include <stdlib.h>

float ** create_array(int n, int m) {
  float ** a = malloc(n * sizeof(float *));
  float * a_ = malloc(n * m * sizeof(float));

  int i, j;

  for (i = 0; i < n; i++) {
    a[i] = a_ + i * m;
    for (j = 0; j < m; j++) {
      a[i][j] = i+j;
    }
  }

  return a;
}

void free_array(float ** a) {
  free(a[0]);
  free(a);
}

void kernel_0(int n, int m, float ** A, float b) {
  int i, j;
}

int main() {
  int i;

  int n = 16;
  int m = 16;
  int p = 16;
  int q = 16;

  float alpha = 3.5;
  float beta = 3.5;

  float ** A = create_array(n, p);
  float ** B = create_array(p, q);
  float ** C = create_array(q, m);

  float ** D = create_array(n, q);
  float ** E = create_array(n, m);

#if ORIGINAL == 1
  #pragma tilek kernel data(A[0:n][0:p], B[0:p][0:q], C[0:q][0:m], D[0:n][0:q], E[0:n][0:m])
  {
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++) {
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < q; j++) {
        D[i][j] = 0;
        for (k = 0; k < p; k++) {
          D[i][j] += A[i][k] * B[k][j];
        }
        D[i][j] *= alpha;
      }
    } // n, p, q, alpha // A, B, D
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++) {
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < m; j++) {
        E[i][j] = 0;
        for (k = 0; k < q; k++) {
          E[i][j] += D[i][k] * C[k][j];
        }
        E[i][j] *= beta;
      }
    } // n, m, q, beta // C, D, E
  }
#else
  struct klt_kernel_t * kernel = klt_build_kernel(0);

  kernel->param[0] = &n;
  kernel->param[1] = &m;
  kernel->param[2] = &p;
  kernel->param[3] = &q;
  kernel->param[4] = &alpha;
  kernel->param[5] = &beta;

  kernel->data[0].ptr = &A[0][0];
  kernel->data[1].ptr = &B[0][0];
  kernel->data[2].ptr = &C[0][0];
  kernel->data[3].ptr = &D[0][0];
  kernel->data[4].ptr = &E[0][0];

  kernel->loops[0].lower = 0;
  kernel->loops[0].upper = n-1;
  kernel->loops[0].stride = 1;

  kernel->loops[1].lower = 0;
  kernel->loops[1].upper = q-1;
  kernel->loops[1].stride = 1;

  kernel->loops[2].lower = 0;
  kernel->loops[2].upper = n-1;
  kernel->loops[2].stride = 1;

  kernel->loops[3].lower = 0;
  kernel->loops[3].upper = m-1;
  kernel->loops[3].stride = 1;

  klt_execute_kernel(kernel);
#endif

  return 0;
}

