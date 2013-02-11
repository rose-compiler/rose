


/* 
   by Didem Unat 
   3D 7-point jacobi
   Written to be used as an input program to mint translator 
  
   See the alloc2D function, which allocates contiguous memory space to 
   the array. 
 */

//#include "common.h"
#include <stdio.h>
#include <math.h>
//#include <omp.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#define REAL double
#define FLOPS 8 
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

  int z, y; 
  for ( z = 0 ; z < nk ; z++, m_tempzy += ny ) {    
    m_buffer[z] = m_tempzy;
    for (  y = 0 ; y < ny ; y++, m_tempzyx += nx ) {
      m_buffer[z][y] = m_tempzyx;
    }
  }

  return m_buffer;
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

//allocate 3D array                                                                                     
REAL ***alloc3D_(int n, int m,int k){

  REAL ***E=NULL;

  int nx=n, ny=m, nk = k;

  E = (REAL***)malloc(sizeof(REAL**)* nk);
  assert(E);
  
  E[0] = (REAL**)malloc(sizeof(REAL*)* nk * ny);
  E[0][0] = (REAL*)malloc(sizeof(REAL)*nx * ny * nk );
  
  int jj,kk;
  
  for(kk=0 ; kk < nk ; kk++){

    if(kk > 0)
      {
	E[kk] = E[kk-1] + ny ;
	E[kk][0] = E[kk-1][0] + ny*nx ;
      }

    for(jj=1; jj< ny; jj++) {
      E[kk][jj] = E[kk][jj-1] + nx ; 
    }
    }
  return(E);
}

void free3D(REAL*** E)
{
  //int k=0;
  /*  for(k=0 ; k < m ; k++)
    {
      free(E[k]);
      }*/
  free(E[0][0]);
  free(E[0]);
  free(E);

}
void init(REAL*** E, int N, int M, int K)
{
  int i,j,k; 

  for(k=0 ; k < K ; k++)
    for(i=0 ; i < M ; i++)
      for(j=0 ; j < N ; j++){
	
      E[k][i][j]=1.0;

      if(i==0 || i == M-1 || j == 0 || j == N-1 || k==0 || k == K-1 )
      	E[k][i][j]=0.0;
    }
}

//calculate l2norm for comparison
void calculatel2Norm(REAL*** E, int N, int M, int K, int nIters)
{
  int i, j, k  =0;

  float mx = -1;
  float l2norm = 0;

  for (k=1; k<= K ; k++){
    for (j=1; j<= M; j++){
      for (i=1; i<= N; i++) {
	l2norm += E[k][j][i]*E[k][j][i];

	if (E[k][j][i] > mx)
	  mx = E[k][j][i];
      }
    }
  }
  l2norm /= (float) ((N)*(M)*(K));
  l2norm = sqrt(l2norm);
  printf(":N %d M %d K %d , iteration %d\n", N, M, K , nIters);                                                                                           
  printf(":max: %20.12e, l2norm: %20.12e\n",mx,l2norm);                                                                                          
}

int main (int argc, char* argv[])
{  

  int n = 256; 
  int m = 256; 
  int k = 256; 

  REAL c0=0.5;
  REAL c1=-0.25;

  REAL*** Unew; REAL*** Uold;

  Unew= alloc3D(n+2, m+2, k+2);
  Uold= alloc3D(n+2, m+2, k+2);
 
  init(Unew, n+2, m+2, k+2);
  init(Uold, n+2, m+2, k+2);

  int T= 20;

  printf("\n=====Timings (sec) for 7-Point Jacobi, Solving Heat Eqn ");
  if(sizeof(REAL) == 4)
    printf(" (Single Precision) =====\n");
  if(sizeof(REAL) == 8)
    printf(" (Double Precision) =====\n");

  printf("Kernel\t Time(sec)\tGflops  \tBW-ideal(GB/s)\tBW-algorithm (N=(%d,%d) ite\
rs=%d)\n", n,n, T);
  printf("------\t----------\t--------\t--------------\t------------\n");


  int nIters = 0;
  double time_elapsed ; 
  double Gflops=0.0;

#pragma mint copy ( Uold, toDevice, ( n+2 ), ( m+2 ), ( k+2 ) )
#pragma mint copy ( Unew, toDevice, ( n+2 ), m+2, ( k+2 ) )
 
#pragma mint parallel 
  {
    time_elapsed = getTime();
    int t=0; 

    while( t < T ){
      t++;
      int x, y, z;     
        
      //7-point stencil
#pragma mint for  nest(all) tile(16,16,16) chunksize(1,1,16) 
      for (z=1; z<= k; z++){
	for (y=1; y<= m; y++){
	  for (x=1; x<= n; x++) {
	    Unew[z][y][x] = c0* Uold[z][y][x]  + c1 * (Uold[z][y][x-1] + Uold[z][y][x+1] +
						       Uold[z][y-1][x] + Uold[z][y+1][x] +
						       Uold[z-1][y][x] + Uold[z+1][y][x]); 
	  }
      }
    }
#pragma mint single
    {
      REAL*** tmp;
      tmp = Uold; Uold = Unew; Unew = tmp;
      nIters = t ; 
    }
    
    }//end of while
  }//end of parallel region
#pragma mint copy(Uold, fromDevice, (n+2), (m+2), (k+2))

  time_elapsed = getTime() - time_elapsed ; 

  Gflops = (double)(nIters * (n) * (m) * (k) * 1.0e-9 * FLOPS) / time_elapsed ;

  printf("%s%3.3f \t%5.3f\n", "Heat3D   ", time_elapsed, Gflops);

  calculatel2Norm(Uold, n, m, k, T);

  
  free3D(Uold);
  free3D(Unew);

  return 0;
}
