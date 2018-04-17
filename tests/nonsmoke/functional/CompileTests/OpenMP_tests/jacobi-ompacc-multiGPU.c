/*
 * Rectangular matrix multiplication, started from MIT Cilk matmul.cilk example
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "omp.h"

#include <pthread.h>
#include <string.h>
#define REAL double
/* in second */
#define read_timer() omp_get_wtime()
/* in ms */
#define read_timer_ms() (omp_get_wtime()*1000.0)

#define MAX_GPU_COUNT 4

int mits=5000;
REAL tol=0.0000000001,relax=1.0,alpha=0.0543; 
REAL error_ref= 9.213041E-04, resid_ref = 2.355794E-08; // depending on MSIZE and precision (double vs. float) !!

void initialize(REAL *u, REAL *uold, REAL *f, REAL* dx, REAL* dy, int m, int n)
{
      
      int i,j, xx,yy;
      //double PI=3.1415926;

      *dx = 2.0 / (n-1);
      *dy = 2.0 / (m-1);

/* Initialize initial condition and RHS */

#pragma omp parallel for private(xx,yy,j,i)
       for (i=0;i<n;i++)
         for (j=0;j<m;j++)      
           {
            xx =(int)( -1.0 + *dx * (i-1));        
            yy = (int)(-1.0 + *dy * (j-1)) ;       
            u[j + i * m] = 0.0;                       
            uold[j + i * m] = 0.0;                       
            f[j + i * m] = -1.0*alpha *(1.0-xx*xx)*(1.0-yy*yy)\
               - 2.0*(1.0-xx*xx)-2.0*(1.0-yy*yy);  
          }

}

double maxerror(REAL *A, REAL *B, int n)
{
    int i, j;
    double error = 0.0;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            double diff = (A[i * n + j] - B[i * n + j]) / A[i * n + j];
//            printf("%4f -- %4f\n", A[i*n+j], B[i*n+j]);
            if (diff < 0)
                diff = -diff;
            if (diff > error)
                error = diff;
        }
    }
    return error;
}

void jacobi_GPU(REAL *u, REAL *uold, REAL *f, REAL dx, REAL dy, int offset, int m, int n, double* error)
{
  double omega;
  int i,j,k;
  double  resid,ax,ay,b;
  //      double  error_local;

  //      float ta,tb,tc,td,te,ta1,ta2,tb1,tb2,tc1,tc2,td1,td2;
  //      float te1,te2;
  //      float second;

  omega=relax;
  /*
   * Initialize coefficients */
  ax = 1.0/(dx*dx); /* X-direction coef */
  ay = 1.0/(dy*dy); /* Y-direction coef */
  b  = -2.0/(dx*dx)-2.0/(dy*dy) - alpha; /* Central coeff */ 

  int tid = omp_get_thread_num();
  double err_tmp = 0.0;
//#pragma omp parallel
    {
//#pragma omp target device (tid) map(tofrom: u[offset*m:m*n]) map(to: uold[offset*m:m*n],f[offset*m:m*n],m,n, offset,ax,ay,b,omega) 
#pragma omp parallel for shared(uold,u,f, offset, ax,ay,b,omega,n) private(resid,j,i) reduction(+:err_tmp)
      for (i=offset+1;i<offset+(n-1);i++) 
      { 
        for (j=1;j<(m-1);j++)   
        {
          resid = (ax*(uold[j + (i-1)*m] + uold[j + (i+1)*m])\
              + ay*(uold[j-1 + i*m] + uold[j+1 + i*m])+ b * uold[j + i*m] - f[j+i*m])/b;  
          u[j+i*m] = uold[j+i*m] - omega * resid;  
          err_tmp = err_tmp + resid*resid ;   
        }
      }
    }
    /*  omp end parallel */
    *error += err_tmp;
}

void jacobi_omp(REAL *u, REAL *uold, REAL *f, REAL dx, REAL dy, int m, int n)
{
  double omega;
  int i,j,k;
  double  error,resid,ax,ay,b;
  //      double  error_local;

  //      float ta,tb,tc,td,te,ta1,ta2,tb1,tb2,tc1,tc2,td1,td2;
  //      float te1,te2;
  //      float second;

  omega=relax;
  /*
   * Initialize coefficients */

  ax = 1.0/(dx*dx); /* X-direction coef */
  ay = 1.0/(dy*dy); /* Y-direction coef */
  b  = -2.0/(dx*dx)-2.0/(dy*dy) - alpha; /* Central coeff */ 

  error = 10.0 * tol;
  k = 1;
  REAL* tmp;

  while ((k<=mits)&&(error>tol)) 
  {
    error = 0.0;    

    /* Copy new solution into old */
    tmp = u;
    u = uold;
    uold = tmp;
#pragma omp parallel
    {
#pragma omp for private(resid,j,i) reduction(+:error) nowait
      for (i=1;i<(n-1);i++)  
        for (j=1;j<(m-1);j++)   
        { 
          resid = (ax*(uold[j + (i-1)*m] + uold[j + (i+1)*m])\
              + ay*(uold[j-1 + i*m] + uold[j+1 + i*m])+ b * uold[j + i*m] - f[j+i*m])/b;  

          u[j+i*m] = uold[j+i*m] - omega * resid;  
          error = error + resid*resid ;   
        }

    }
    /*  omp end parallel */

    /* Error check */

    k = k + 1;
    if (k%500==0) 
      printf("OMP_run: finished %d iteration.\n",k);
    error = sqrt(error)/(n*m);
//    printf("%d %e %e\n",k,error,tol);
  }          /*  End iteration loop */

  printf("Total Number of Iterations:%d\n",k); 
  printf("Residual:%E\n", error); 
  printf("Residual_ref :%E\n", resid_ref);
  printf ("Diff ref=%E\n", fabs(error-resid_ref));
  assert (fabs(error-resid_ref)/resid_ref < 1E-5);
}

int main(int argc, char *argv[])
{
    int m,n, idev;
    int num_threads;
    REAL *u, *uold, *f;
    REAL dx,dy;
    double omp_for_elapsed, acc_elapsed;
    int halosize = 1; 
    

//    if (argc != 3) {
//        fprintf(stderr, "Usage: matmul <m> <n>\n");
//        exit(1);
//    }
//    m = atoi(argv[1]);
//    n = atoi(argv[2]);
    m = 512;
    n = 512;
    
    u = (REAL*)malloc(m * n * sizeof(REAL));
    uold = (REAL*)malloc(m * n * sizeof(REAL));
    f = (REAL*)malloc(m * n * sizeof(REAL));

    /* openmp parallel for version */
    initialize(u, uold, f, &dx, &dy, m, n);
    omp_for_elapsed = omp_get_wtime();
    jacobi_omp(u, uold, f, dx, dy, m,n);
    omp_for_elapsed = omp_get_wtime() - omp_for_elapsed;


    initialize(u, uold, f, &dx, &dy, m, n);
    int GPU_N = 0;
    cudaGetDeviceCount(&GPU_N);
    if (GPU_N > MAX_GPU_COUNT)
    {
        GPU_N = MAX_GPU_COUNT;
    }
    printf("CUDA-capable device count: %i\n", GPU_N);
    omp_set_num_threads(GPU_N); 
#pragma omp parallel 
    {
#pragma omp master
        { num_threads = omp_get_num_threads();
        }

    }
    //double ompacc_time = read_timer();
    acc_elapsed = omp_get_wtime();
    REAL* tmp;
    double* error;
    double error_sum;
    error = (double*)malloc(sizeof(double)*GPU_N);
    #pragma omp parallel shared (GPU_N, u, uold, f, m, n, error,error_sum) private(idev)
//    for (idev = 0; idev < GPU_N; idev++)
    {
      int tid = omp_get_thread_num();
      cudaSetDevice(tid);
      int size = n / GPU_N;
      int offset = size * tid;
      if(tid < n%GPU_N)
      {
        size++; 
      }
      if(tid >= n%GPU_N)
        offset += n%GPU_N;
      else
        offset += tid;
      if(tid != 0)
        offset = offset - halosize;
      size = size + halosize;
      if(tid != GPU_N-1 && tid != 0)
        size = size + halosize;

      printf("thread %d working on GPU devices %d with size %d copying data from y_ompacc with offset %d\n",tid, tid, size,offset);
    int k;
    k = 1;
    error_sum = 10.0 * tol;
    /* Copy new solution into old */
    while ((k<=mits)&&(error_sum>tol)) 
    {
#pragma omp barrier
#pragma omp master
{
      tmp = u;
      u = uold;
      uold = tmp;
      error_sum = 0.0;
} 
      error[tid] = 0.0;   
#pragma omp barrier
      jacobi_GPU(u, uold, f, dx, dy, offset, m,size,&error[tid]);
    /* Error check */

    k = k + 1;
#pragma omp master
{
    if (k%500==0) 
      printf("GPU_run: finished %d iteration.\n",k);
}
#pragma omp critical
{ 
    error_sum += error[tid];
//    printf("thread %d error = %f\n",tid, error_sum);
}
#pragma omp barrier
#pragma omp master
{ 
    error_sum = sqrt(error_sum)/(n*m);
//    printf("%d %e %e\n",k,error_sum,tol);
}
    }          /*  End iteration loop */

#pragma omp critical 
#pragma omp master
    {
      printf("Total Number of Iterations:%d\n",k); 
      printf("Residual:%E\n", error_sum); 
      printf("Residual_ref :%E\n", resid_ref);
      printf ("Diff ref=%E\n", fabs(error_sum-resid_ref));
      assert (fabs(error_sum-resid_ref)/resid_ref < 1E-5);
    }
    }    // end of idev loop

    acc_elapsed = omp_get_wtime() - acc_elapsed;
    free(error);

    printf("=======================================================================\n");
    printf("\t\tmatmul(%dx%d) example on %d threads(cores)\n", n, n, num_threads);
    printf("-----------------------------------------------------------------------\n");
    printf("Performance:  Runtime (s)\t MFLOPS\t\t\t Error\n");
    printf("-----------------------------------------------------------------------\n");
//    printf("Sequential      :  %4f \t\t %4f\t\t%g\n", seq_elapsed, 2.0 * n * n * n / (1.0e6 * (seq_elapsed)), maxerror(C_seq, C_seq, n));
    printf("OMP For         :  %4f \t\t %4f\t\t\n", omp_for_elapsed, 2.0 * n * n * n / (1.0e6 * (omp_for_elapsed)));
    printf("ACC For         :  %4f \t\t %4f\t\t\n", acc_elapsed, 2.0 * n * n * n / (1.0e6 * (acc_elapsed)));

    free(u);
    free(uold);
    free(f);
    return 0;
}
