/* An example code 
 *
 * */
#include <stdio.h>
#include <math.h>
#include <omp.h> 
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

int main()
{
//  float toler;
/*      printf("Input n,m (< %d) - grid dimension in x,y direction:\n",MSIZE); 
      scanf ("%d",&n);
      scanf ("%d",&m);
      printf("Input tol - error tolerance for iterative solver\n"); 
      scanf("%f",&toler);
      tol=(double)toler;
      printf("Input mits - Maximum iterations for solver\n"); 
      scanf("%d",&mits);
*/
  n = 200;
  m = 200;
  tol = 0.0000000001;
  mits = 1000;
  driver();
  return 1;
}
/*************************************************************
* Subroutine driver () 
* This is where the arrays are allocated and initialzed. 
*
* Working varaibles/arrays 
*     dx  - grid spacing in x direction 
*     dy  - grid spacing in y direction 
*************************************************************/

void driver()
{
  initialize();
/* Solve Helmholtz equation */
  jacobi();
/* error_check (n,m,alpha,dx,dy,u,f) */
  error_check();
}
/*      subroutine initialize (n,m,alpha,dx,dy,u,f) 
******************************************************
* Initializes data 
* Assumes exact solution is u(x,y) = (1-x^2)*(1-y^2)
*
******************************************************/

void initialize()
{
  int i;
  int j;
  int xx;
  int yy;
//  double PI = 3.1415926;
  dx = 2.0 / (n - 1);
// -->dx@112:2
  dy = 2.0 / (m - 1);
//-->dy@113:2
/* Initialize initial condition and RHS */
//#pragma omp parallel for private(i,j,xx,yy)
  
#pragma omp parallel for private (xx,yy,i,j) firstprivate (n,m)
  for (i = 0; i <= n - 1; i += 1) {
    
#pragma omp parallel for private (xx,yy,j) firstprivate (alpha,dx,dy)
    for (j = 0; j <= m - 1; j += 1) {
      xx = ((int )(- 1.0 + dx * (i - 1)));
/* -1 < x < 1 */
      yy = ((int )(- 1.0 + dy * (j - 1)));
/* -1 < y < 1 */
      u[i][j] = 0.0;
      f[i][j] = - 1.0 * alpha * (1.0 - (xx * xx)) * (1.0 - (yy * yy)) - 2.0 * (1.0 - (xx * xx)) - 2.0 * (1.0 - (yy * yy));
    }
  }
}
/*      subroutine jacobi (n,m,dx,dy,alpha,omega,u,f,tol,maxit)
******************************************************************
* Subroutine HelmholtzJ
* Solves poisson equation on rectangular grid assuming : 
* (1) Uniform discretization in each direction, and 
* (2) Dirichlect boundary conditions 
* 
* Jacobi method is used in this routine 
*
* Input : n,m   Number of grid points in the X/Y directions 
*         dx,dy Grid spacing in the X/Y directions 
*         alpha Helmholtz eqn. coefficient 
*         omega Relaxation factor 
*         f(n,m) Right hand side function 
*         u(n,m) Dependent variable/Solution
*         tol    Tolerance for iterative solver 
*         maxit  Maximum number of iterations 
*
* Output : u(n,m) - Solution 
*****************************************************************/

void jacobi()
{
  double omega;
  int i;
  int j;
  int k;
  double error;
  double resid;
  double ax;
  double ay;
  double b;
  omega = relax;
/*
* Initialize coefficients */
  ax = 1.0 / (dx * dx);
/* X-direction coef */
  ay = 1.0 / (dy * dy);
/* Y-direction coef */
  b = - 2.0 / (dx * dx) - 2.0 / (dy * dy) - alpha;
/* Central coeff */
  error = 10.0 * tol;
  k = 1;
  while(k <= mits && error > tol){
    error = 0.0;
/* Copy new solution into old */
//#pragma omp parallel
{
//#pragma omp for private(i,j)
      
#pragma omp parallel for private (i,j)
      for (i = 0; i <= n - 1; i += 1) {
        
#pragma omp parallel for private (j)
        for (j = 0; j <= m - 1; j += 1) {
          uold[i][j] = u[i][j];
        }
      }
//#pragma omp for private(i,j,resid) reduction(+:error) nowait
      
#pragma omp parallel for private (resid,i,j) reduction (+:error)
      for (i = 1; i <= n - 1 - 1; i += 1) {
        
#pragma omp parallel for private (resid,j) reduction (+:error) firstprivate (omega,ax,ay,b)
        for (j = 1; j <= m - 1 - 1; j += 1) {
          resid = (ax * (uold[i - 1][j] + uold[i + 1][j]) + ay * (uold[i][j - 1] + uold[i][j + 1]) + b * uold[i][j] - f[i][j]) / b;
          u[i][j] = uold[i][j] - omega * resid;
          error = error + resid * resid;
        }
      }
    }
/*  omp end parallel */
/* Error check */
//  k = k + 1;
    error = sqrt(error) / (n * m);
  }
/*  End iteration loop */
  printf("Total Number of Iterations:%d\n",k);
  printf("Residual:%E\n",error);
}

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
