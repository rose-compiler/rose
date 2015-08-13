#include <stdio.h>
#include <math.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// Add timing support
#include <sys/time.h>
double time_stamp()
{
  struct timeval t;
  double time;
  gettimeofday(&t, NULL);
  time = t.tv_sec + 1.0e-6*t.tv_usec;
  return time;
}
double time1, time2;

void driver(void);
void initialize(void);
void jacobi(void);
void error_check(void);

/************************************************************
* program to solve a finite difference 
* discretization of Helmholtz equation :  
* (d2/dx2)u + (d2/dy2)u - alpha u = f 
* using Jacobi iterative method. 
*
* Modified: Sanjiv Shah,       Kuck and Associates, Inc. (KAI), 1998
* Author:   Joseph Robicheaux, Kuck and Associates, Inc. (KAI), 1998
*
* This C version program is translated by 
* Chunhua Liao, University of Houston, Jan, 2005 
* 
* Directives are used in this code to achieve parallelism. 
* All do loops are parallelized with default 'static' scheduling.
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
 int n,m,mits; 
 double tol,relax=1.0,alpha=0.0543; 
 double u[MSIZE][MSIZE],f[MSIZE][MSIZE],uold[MSIZE][MSIZE];
 double dx,dy;

int chiterations = 0;
int chloads = 0;
int chstores = 0;
int chflops = 0;

/*      subroutine initialize (n,m,alpha,dx,dy,u,f) 
******************************************************
* Initializes data 
* Assumes exact solution is u(x,y) = (1-x^2)*(1-y^2)
*
******************************************************/

void initialize( )
{
      
      int i,j, xx,yy;
      //double PI=3.1415926;

      dx = 2.0 / (n-1);
      dy = 2.0 / (m-1);
#if 0
/* Initialize initial condition and RHS */
chiterations = n*m;
#pragma aitool fp_plus(2) fp_multiply(2)
       for (i=0;i<n;i++)
         for (j=0;j<m;j++)      
           {
            xx =(int)( -1.0 + dx * (i-1));        
            yy = (int)(-1.0 + dy * (j-1)) ;       
            u[i][j] = 0.0;                       
          }


chiterations = n*m;
#pragma aitool fp_minus(6) fp_multiply(5) 
       for (i=0;i<n;i++)
         for (j=0;j<m;j++)      
           {
            u[i][j] = 0.0;                       
            f[i][j] = -1.0*alpha *(1.0-xx*xx)*(1.0-yy*yy)\
               - 2.0*(1.0-xx*xx)-2.0*(1.0-yy*yy);  
          }
#endif
chiterations = n*m;
#pragma aitool fp_plus(2) fp_minus(6) fp_multiply(7) 
       for (i=0;i<n;i++)
         for (j=0;j<m;j++)      
           {
            xx =(int)( -1.0 + dx * (i-1));        
            yy = (int)(-1.0 + dy * (j-1)) ;       
            u[i][j] = 0.0;                       
            f[i][j] = -1.0*alpha *(1.0-xx*xx)*(1.0-yy*yy)\
               - 2.0*(1.0-xx*xx)-2.0*(1.0-yy*yy);  
          }
}
