#include <stdio.h>
#include <math.h>
#include "omp.h" 
void driver();
void initialize();
void jacobi();
void error_check();
#define MSIZE 200
int n;
int m;
int mits;
double tol;
double relax = 1.0;
double alpha = 0.0543;
double u[200][200];
double f[200][200];
double uold[200][200];
double dx;
double dy;

void error_check()
{
  int i;
  int j;
  double xx;
  double yy;
  double temp;
  double error;
  dx = 2.0 / (n - 1);
  dy = 2.0 / (m - 1);
  error = 0.0;
//#pragma omp parallel for private(i,j,xx,yy,temp) reduction(+:error)
  
#pragma omp parallel for private (xx,yy,temp,i,j) reduction (+:error)
  for (i = 0; i <= n - 1; i += 1) {
    
#pragma omp parallel for private (xx,yy,temp,j) reduction (+:error) firstprivate (dx,dy)
    for (j = 0; j <= m - 1; j += 1) {
      xx = - 1.0 + dx * (i - 1);
      yy = - 1.0 + dy * (j - 1);
      temp = u[i][j] - (1.0 - xx * xx) * (1.0 - yy * yy);
      error = error + temp * temp;
    }
  }
  error = sqrt(error) / (n * m);
  printf("Solution Error :%E \n",error);
}
