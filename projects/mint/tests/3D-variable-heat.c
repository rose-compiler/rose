

/* 
   Written by Xing Cai, Nov-2010
   Modified by Didem Unat, Dec-2010

   A test program for solving the 3D heat conduction problem
         -div ( kappa(x,y,z) grad u ) = f(x,y,z)
   where kappa(x,y,z) = 0.5+0.45*sin(pi*x)*sin(pi*y)*sin(pi*z).
   Note that the heat conduction coefficient is a variable field.
   An explicit time stepping scheme is used.

*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

//#include <omp.h>
#include <sys/time.h>
#include <assert.h>

#define FLOPS 26.0
#define REAL double
#define chunk 64 
const double kMicro = 1.0e-6;

REAL ***alloc3D(int n, int m,int k)
{
  REAL ***m_buffer=NULL;

  int nx=n, ny=m, nk = k;

  m_buffer = (REAL***)malloc(sizeof(REAL**)* nk);
  assert(m_buffer);  
  
  REAL** m_tempzy = (REAL**)malloc(sizeof(REAL*)* nk * ny);
  REAL *m_tempzyx = (REAL*)malloc(sizeof(REAL)* nx * ny * nk );
  
  for ( int z = 0 ; z < nk ; z++, m_tempzy += ny ) {    
    m_buffer[z] = m_tempzy;
    for ( int y = 0 ; y < ny ; y++, m_tempzyx += nx ) {
      m_buffer[z][y] = m_tempzyx;
    }
  }

  return m_buffer;
}


void free3D(REAL*** E)
{
  free(E[0][0]);
  free(E[0]);
  free(E);

}


double getTime()
{
  struct timeval TV;

  const int RC = gettimeofday(&TV, NULL);
  if(RC == -1)
    {
      printf("ERROR: Bad call to gettimeofday\n");
      return(-1);
    }

  return( ((double)TV.tv_sec) + kMicro * ((double)TV.tv_usec) );

}  // end getTime()                                                                               

int main (int nargs, char** args)
{
  int n;      /* number of points in each direction */
  double h;   /* grid spacing, same in all the directions */
  double ***u_old, ***u_new, ***rhs, ***kappa;
  double factor, l2_norm, dt, T;
  int i,j,k;
  int max_iters=20;

  if (nargs>1)
    n = atoi(args[1]);
  else
    n = 256;

  T = 1.0;
  h = 1.0/(n-1);
  dt = h*h/6.0;   /* a safe choice of dt when k(x,y,z)<1 */

  u_old = alloc3D(n+2, n+2, n+2);
  u_new = alloc3D(n+2, n+2, n+2);
  rhs = alloc3D(n+2, n+2, n+2);
  kappa = alloc3D(n+2, n+2, n+2);
 
  /* fill values of rhs */
  for (k=0; k<= n+1; k++)
    for (j=0; j<= n+1; j++)
      for (i=0; i<= n+1; i++)
        rhs[k][j][i] = dt*(1.0+cos(M_PI*i*h)*cos(M_PI*j*h)*cos(M_PI*k*h));

  /* fill values of kappa */
  for (k=0; k<= n+1; k++)
    for (j=0; j<= n+1; j++)
      for (i=0; i<= n+1; i++)
        kappa[k][j][i] = 0.5+0.45*sin(M_PI*i*h)*sin(M_PI*j*h)*sin(M_PI*k*h);

  /* fill initial values */
  for (k=0; k<= n+1; k++)
    for (j=0; j<= n+1; j++)
      for (i=0; i<= n+1; i++) {
	u_old[k][j][i] = sin(M_PI*i*h)*sin(M_PI*j*h)*sin(M_PI*k*h);
	u_new[k][j][i] = 0.;
      }


  /* main time loop */
  int nIters=0; 

  factor = dt/h/h/2.0;

  printf("\n=====Timings (sec) for 3D variable-coefficient Eqn-7 point ");
  if(sizeof(REAL) == 4)
    printf(" (Single Precision) =====\n");
  if(sizeof(REAL) == 8)
    printf(" (Double Precision) =====\n");

  printf("Kernel\t Time(sec)\tGflops  \tBW-ideal(GB/s)\tBW-algorithm (N=(%d) iters=%d)\n",n, max_iters);
  printf("------\t----------\t--------\t--------------\t------------\n");



  double time_elapsed= getTime();
  double Gflops =0.0; 


#pragma mint copy ( u_old,  toDevice, ( n+2 ), n+2, ( n+2 ))
#pragma mint copy ( u_new,  toDevice, ( n+2 ), n+2, ( n+2 ))
#pragma mint copy ( rhs, toDevice, ( n+2 ), n+2, ( n+2 ))
#pragma mint copy ( kappa, toDevice, ( n+2 ), n+2, ( n+2 ))

#pragma mint parallel default(shared)
  {
    int iters = 0 ; 

    double t=0.0;

  while (t < T && iters < max_iters) {

    t += dt ; 
    ++iters;

    /* update each interior point */
#pragma mint for  nest(all)  tile ( 16, 16, 1 )
    for (k=1; k<= n; k++){
      for (j=1; j<= n; j++){
	for (i=1; i<= n; i++)
	  u_new[k][j][i] = u_old[k][j][i] + rhs[k][j][i] 
	    + factor *((kappa[k][j][i+1]+ kappa[k][j][i])  *(u_old[k][j][i+1]-u_old[k][j][i])
		     + (kappa[k][j][i]  + kappa[k][j][i-1])*(u_old[k][j][i]-u_old[k][j][i-1])
		     + (kappa[k][j+1][i]+ kappa[k][j][i])  *(u_old[k][j+1][i]-u_old[k][j][i])
		     + (kappa[k][j][i]  + kappa[k][j-1][i])*(u_old[k][j][i]-u_old[k][j-1][i])
		     + (kappa[k+1][j][i]+ kappa[k][j][i])  *(u_old[k+1][j][i]-u_old[k][j][i])
		     + (kappa[k][j][i]  + kappa[k-1][j][i])*(u_old[k][j][i]-u_old[k-1][j][i]));
	}}
    /* pointer swap */
  #pragma mint single
    {
      REAL*** tmp;
      tmp = u_old; u_old= u_new; u_new = tmp;

      nIters = iters; 
    }
  }
  }

#pragma mint copy ( u_old, fromDevice, ( n+2 ), ( n+2 ), ( n+2 ))

  time_elapsed = getTime() - time_elapsed;

  Gflops = (double)(nIters * (n) * (n) * (n) * 1.0e-9 * FLOPS) / time_elapsed ;

  l2_norm = 0;
  for (k=0; k<= n+1; k++)
    for (j=0; j<= n+1; j++)
      for (i=0; i<= n+1; i++) {
	factor = sin(M_PI*i*h)*sin(M_PI*j*h)*sin(M_PI*k*h);
	l2_norm += (factor-u_old[k][j][i])*(factor-u_old[k][j][i]);
      }

  printf("%s%3.3f \t%5.3f\n", "VariableHeat   ", time_elapsed, Gflops);
  printf(":N %d M %d K %d , iteration %d\n", n, n, n , nIters);
  printf(":max: %20.12e, l2norm: %20.12e\n",factor,sqrt(l2_norm*h*h*h));   

 
  free3D(u_new);
  free3D(u_old);
  free3D(rhs);
  free3D(kappa);

  return 0;
}
