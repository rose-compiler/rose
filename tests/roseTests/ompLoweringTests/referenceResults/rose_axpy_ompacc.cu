// Experimental test input for Accelerator directives
// Liao 1/15/2013
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/* change this to do saxpy or daxpy : single precision or double precision*/
#define REAL double
#define VEC_LEN 1024000 //use a fixed number for now
/* zero out the entire vector */
#include "libxomp.h" 
#include "xomp_cuda_lib_inlined.cu" 

void zero(double *A,int n)
{
  int i;
  for (i = 0; i < n; i++) {
    A[i] = 0.0;
  }
}
/* initialize a vector with random floating point numbers */

void init(double *A,int n)
{
  int i;
  for (i = 0; i < n; i++) {
    A[i] = drand48();
  }
}

__global__ void OUT__1__10001__(int n,double a,double *_dev_x,double *_dev_y)
{
  int _p_i;
  int _dev_i = blockDim.x * blockIdx.x + threadIdx.x;
  if (_dev_i >= 0 && _dev_i <= n - 1) {
    _dev_y[_dev_i] += (a * _dev_x[_dev_i]);
  }
}

void axpy_ompacc(double *x,double *y,int n,double a)
{
  int i;
/* this one defines both the target device name and data environment to map to,
   I think here we need mechanism to tell the compiler the device type (could be multiple) so that compiler can generate the codes of different versions; 
   we also need to let the runtime know what the target device is so the runtime will chose the right function to call if the code are generated 
   #pragma omp target device (gpu0) map(x, y) 
*/
{
    double *_dev_x;
    int _dev_x_size = sizeof(double ) * (n - 0);
    _dev_x = ((double *)(xomp_deviceMalloc(_dev_x_size)));
    xomp_memcpyHostToDevice(((void *)_dev_x),((const void *)x),_dev_x_size);
    double *_dev_y;
    int _dev_y_size = sizeof(double ) * (n - 0);
    _dev_y = ((double *)(xomp_deviceMalloc(_dev_y_size)));
    xomp_memcpyHostToDevice(((void *)_dev_y),((const void *)y),_dev_y_size);
/* Launch CUDA kernel ... */
    int _threads_per_block_ = xomp_get_maxThreadsPerBlock();
    int _num_blocks_ = xomp_get_max1DBlock(n - 1 - 0 + 1);
    OUT__1__10001__<<<_num_blocks_,_threads_per_block_>>>(n,a,_dev_x,_dev_y);
    xomp_freeDevice(_dev_x);
    xomp_memcpyDeviceToHost(((void *)y),((const void *)_dev_y),_dev_y_size);
    xomp_freeDevice(_dev_y);
  }
}

int main(int argc,char *argv[])
{
  int n;
  double *y_ompacc;
  double *x;
  double a = 123.456;
  n = 1024000;
  y_ompacc = ((double *)(malloc((n * sizeof(double )))));
  x = ((double *)(malloc((n * sizeof(double )))));
  srand48((1 << 12));
  init(x,n);
  init(y_ompacc,n);
/* openmp acc version */
  axpy_ompacc(x,y_ompacc,n,a);
  free(y_ompacc);
  free(x);
  return 0;
}
