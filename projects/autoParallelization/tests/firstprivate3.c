#include <stdio.h>
#include <math.h>

void driver (void);
void initialize (void);
void jacobi (void);
void error_check (void);

#define MSIZE 200
int n, m, mits;
double tol, relax = 1.0, alpha = 0.0543;
double u[MSIZE][MSIZE], f[MSIZE][MSIZE], uold[MSIZE][MSIZE];
double dx, dy;

void
error_check ()
{

  int i, j;
  double xx, yy, temp, error;

  dx = 2.0 / (n - 1);
  dy = 2.0 / (m - 1);
  error = 0.0;

//#pragma omp parallel for private(i,j,xx,yy,temp) reduction(+:error)
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      {
        xx = -1.0 + dx * (i - 1);
        yy = -1.0 + dy * (j - 1);
        temp = u[i][j] - (1.0 - xx * xx) * (1.0 - yy * yy);
        error = error + temp * temp;
      }
  error = sqrt (error) / (n * m);

  printf ("Solution Error :%E \n", error);

}

