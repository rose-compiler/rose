/* OUTLINED FUNCTION PROTOTYPE */
void OUT_3_jacobi_134(void **__out_argv);
/* OUTLINED FUNCTION PROTOTYPE */
void OUT_2_jacobi_199(void **__out_argv);
/* OUTLINED FUNCTION PROTOTYPE */
void OUT_1_jacobi_242(void **__out_argv);
#include <stdio.h>
#include <math.h>
// Add timing support
#include <sys/time.h>

double diff_ratio(double val,double ref,int significant_digits)
{
  significant_digits >= 1?((void )0) : (__assert_fail("significant_digits>=1","jacobi.c",15,__PRETTY_FUNCTION__));
  double diff_ratio = fabs(val - ref) / fabs(ref);
// 1.0/(double(10^significant_digits)) ;
  double upper_limit = pow(0.1,significant_digits);
  if (diff_ratio >= upper_limit) 
    printf("value :%E  ref_value: %E  diff_ratio: %E >= upper_limit: %E \n",val,ref,diff_ratio,upper_limit);
  diff_ratio < upper_limit?((void )0) : (__assert_fail("diff_ratio < upper_limit","jacobi.c",20,__PRETTY_FUNCTION__));
  return diff_ratio;
}

inline void time_stamp(char *notes)
{
  struct timeval t;
  double time1;
  gettimeofday(&t,((void *)0));
  time1 = t . tv_sec + 1.0e-6 * t . tv_usec;
  printf("%s, %f\n",notes,time1);
}
struct timeval t;
double time1;
double time2;
void driver();
void initialize();
void jacobi();
void error_check();
/************************************************************
* program to solve a finite difference 
* discretization of Helmholtz equation :  
* (d2/dx2)u + (d2/dy2)u - alpha u = f 
* using Jacobi iterative method. 
*
* Modified: Sanjiv Shah,       Kuck and Associates, Inc. (KAI), 1998
* Author:   Joseph Robicheaux, Kuck and Associates, Inc. (KAI), 1998
* This c version program is translated by 
* Chunhua Liao, University of Houston, Jan, 2005 
* 
* Directives are used in this code to achieve paralleism. 
* All do loops are parallized with default 'static' scheduling.
* 
* Input :  n - grid dimension in x direction 
*          m - grid dimension in y direction
*          alpha - Helmholtz constant (always greater than 0.0)
*          tol   - error tolerance for iterative solver
*          relax - Successice over relaxation parameter
*          mits  - Maximum iterations for iterative solver
*
* On output 
*       : u(n,m) - Dependent variable (solutions)
*       : f(n,m) - Right hand side function 
*************************************************************/
 #define MSIZE 500
int n;
int m;
int mits;
double tol;
double relax = 1.0;
double alpha = 0.0543;
double u[500][500];
double f[500][500];
double uold[500][500];
double dx;
double dy;

int main()
{
  float toler;
/*      
          printf("Input n,m (< %d) - grid dimension in x,y direction:\n",MSIZE); 
          scanf ("%d",&n);
          scanf ("%d",&m);
          printf("Input tol - error tolerance for iterative solver\n"); 
          scanf("%f",&toler);
          tol=(double)toler;
          printf("Input mits - Maximum iterations for solver\n"); 
          scanf("%d",&mits);
          */
  n = 500;
  m = 500;
  tol = 0.0000000001;
  mits = 5000;
  driver();
  return 0;
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
  gettimeofday(&t,((void *)0));
  time1 = t . tv_sec + 1.0e-6 * t . tv_usec;
/* Solve Helmholtz equation */
  jacobi();
  gettimeofday(&t,((void *)0));
  time2 = t . tv_sec + 1.0e-6 * t . tv_usec;
  printf("------------------------\n");
  printf("time = %f\n",time2 - time1);
/* error_check (n,m,alpha,dx,dy,u,f)*/
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
//double PI=3.1415926;
  dx = 2.0 / (n - 1);
  dy = 2.0 / (m - 1);
/* Initialize initial condition and RHS */
  
#pragma rose_outline
  for (i = 0; i < n; i++) 
    for (j = 0; j < m; j++) {
/* -1 < x < 1 */
      xx = ((int )((- 1.0) + dx * (i - 1)));
/* -1 < y < 1 */
      yy = ((int )((- 1.0) + dy * (j - 1)));
      u[i][j] = 0.0;
      f[i][j] = (- 1.0) * alpha * (1.0 - (xx * xx)) * (1.0 - (yy * yy)) - 2.0 * (1.0 - (xx * xx)) - 2.0 * (1.0 - (yy * yy));
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
//      double  error_local;
//      float ta,tb,tc,td,te,ta1,ta2,tb1,tb2,tc1,tc2,td1,td2;
//      float te1,te2;
//      float second;
  omega = relax;
/*
   * Initialize coefficients */
/* X-direction coef */
  ax = 1.0 / (dx * dx);
/* Y-direction coef */
  ay = 1.0 / (dy * dy);
/* Central coeff */
  b = (- 2.0) / (dx * dx) - 2.0 / (dy * dy) - alpha;
  error = 10.0 * tol;
  k = 1;
  while(k <= mits && error > tol){
    error = 0.0;
/* Copy new solution into old */
    for (i = 0; i < n; i++) 
      for (j = 0; j < m; j++) 
        uold[i][j] = u[i][j];
    
#pragma rose_outline
    for (i = 1; i < n - 1; i++) 
      for (j = 1; j < m - 1; j++) {
        resid = (ax * (uold[i - 1][j] + uold[i + 1][j]) + ay * (uold[i][j - 1] + uold[i][j + 1]) + b * uold[i][j] - f[i][j]) / b;
        u[i][j] = uold[i][j] - omega * resid;
//        if ((i+j)%500==0) 
//          printf("test something here. \n");
        error = error + resid * resid;
      }
/* Error check */
    k = k + 1;
    if (k % 500 == 0) 
      printf("Finished %d iteration.\n",k);
    error = sqrt(error) / (n * m);
/*  End iteration loop */
  }
  printf("Total Number of Iterations:%d\n",k);
  printf("Residual:%E\n",error);
  diff_ratio(error,2.512265E-08,6);
  printf("Residual's Correctness verification passed.\n");
}
/*      subroutine error_check (n,m,alpha,dx,dy,u,f) 
      implicit none 
************************************************************
* Checks error between numerical and exact solution 
*
************************************************************/

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
  
#pragma rose_outline
  for (i = 0; i < n; i++) 
    for (j = 0; j < m; j++) {
      xx = (- 1.0) + dx * (i - 1);
      yy = (- 1.0) + dy * (j - 1);
      temp = u[i][j] - (1.0 - xx * xx) * (1.0 - yy * yy);
      error = error + temp * temp;
    }
  error = sqrt(error) / (n * m);
  printf("Solution Error :%E \n",error);
  diff_ratio(error,9.378232E-04,6);
  printf("Solution Error's Correctness verification passed.\n");
}
/* REQUIRED CPP DIRECTIVES */
/* REQUIRED DEPENDENT DECLARATIONS */
/* OUTLINED FUNCTION */

void OUT_1_jacobi_242(void **__out_argv)
{
  int n =  *((int *)__out_argv[10]);
  int m =  *((int *)__out_argv[9]);
  double (*u)[500][500] = (double (*)[500][500])__out_argv[8];
  double dx =  *((double *)__out_argv[7]);
  double dy =  *((double *)__out_argv[6]);
  int i =  *((int *)__out_argv[5]);
  int j =  *((int *)__out_argv[4]);
  double xx =  *((double *)__out_argv[3]);
  double yy =  *((double *)__out_argv[2]);
  double temp =  *((double *)__out_argv[1]);
  double error =  *((double *)__out_argv[0]);
  for (i = 0; i < n; i++) 
    for (j = 0; j < m; j++) {
      xx = - 1.0 + dx * ((double )(i - 1));
      yy = - 1.0 + dy * ((double )(j - 1));
      temp = ( *u)[i][j] - (1.0 - xx * xx) * (1.0 - yy * yy);
      error = error + temp * temp;
    }
   *((double *)__out_argv[0]) = error;
   *((double *)__out_argv[1]) = temp;
   *((double *)__out_argv[2]) = yy;
   *((double *)__out_argv[3]) = xx;
   *((int *)__out_argv[4]) = j;
   *((int *)__out_argv[5]) = i;
}
/* REQUIRED CPP DIRECTIVES */
/* REQUIRED DEPENDENT DECLARATIONS */
/* OUTLINED FUNCTION */

void OUT_2_jacobi_199(void **__out_argv)
{
  int n =  *((int *)__out_argv[12]);
  int m =  *((int *)__out_argv[11]);
  double (*u)[500][500] = (double (*)[500][500])__out_argv[10];
  double (*f)[500][500] = (double (*)[500][500])__out_argv[9];
  double (*uold)[500][500] = (double (*)[500][500])__out_argv[8];
  double omega =  *((double *)__out_argv[7]);
  int i =  *((int *)__out_argv[6]);
  int j =  *((int *)__out_argv[5]);
  double error =  *((double *)__out_argv[4]);
  double resid =  *((double *)__out_argv[3]);
  double ax =  *((double *)__out_argv[2]);
  double ay =  *((double *)__out_argv[1]);
  double b =  *((double *)__out_argv[0]);
  for (i = 1; i < n - 1; i++) 
    for (j = 1; j < m - 1; j++) {
      resid = (ax * (( *uold)[i - 1][j] + ( *uold)[i + 1][j]) + ay * (( *uold)[i][j - 1] + ( *uold)[i][j + 1]) + b * ( *uold)[i][j] - ( *f)[i][j]) / b;
      ( *u)[i][j] = ( *uold)[i][j] - omega * resid;
//        if ((i+j)%500==0) 
//          printf("test something here. \n");
      error = error + resid * resid;
    }
   *((double *)__out_argv[3]) = resid;
   *((double *)__out_argv[4]) = error;
   *((int *)__out_argv[5]) = j;
   *((int *)__out_argv[6]) = i;
}
/* REQUIRED CPP DIRECTIVES */
/* REQUIRED DEPENDENT DECLARATIONS */
/* OUTLINED FUNCTION */

void OUT_3_jacobi_134(void **__out_argv)
{
  int n =  *((int *)__out_argv[10]);
  int m =  *((int *)__out_argv[9]);
  double alpha =  *((double *)__out_argv[8]);
  double (*u)[500][500] = (double (*)[500][500])__out_argv[7];
  double (*f)[500][500] = (double (*)[500][500])__out_argv[6];
  double dx =  *((double *)__out_argv[5]);
  double dy =  *((double *)__out_argv[4]);
  int i =  *((int *)__out_argv[3]);
  int j =  *((int *)__out_argv[2]);
  int xx =  *((int *)__out_argv[1]);
  int yy =  *((int *)__out_argv[0]);
  for (i = 0; i < n; i++) 
    for (j = 0; j < m; j++) {
/* -1 < x < 1 */
      xx = ((int )(- 1.0 + dx * ((double )(i - 1))));
/* -1 < y < 1 */
      yy = ((int )(- 1.0 + dy * ((double )(j - 1))));
      ( *u)[i][j] = 0.0;
      ( *f)[i][j] = - 1.0 * alpha * (1.0 - ((double )(xx * xx))) * (1.0 - ((double )(yy * yy))) - 2.0 * (1.0 - ((double )(xx * xx))) - 2.0 * (1.0 - ((double )(yy * yy)));
    }
   *((int *)__out_argv[0]) = yy;
   *((int *)__out_argv[1]) = xx;
   *((int *)__out_argv[2]) = j;
   *((int *)__out_argv[3]) = i;
}
