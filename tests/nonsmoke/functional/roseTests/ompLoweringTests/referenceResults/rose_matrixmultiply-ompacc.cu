/*
Naive matrix-matrix multiplication(mmm)
By C. Liao
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#define N 1024 
#define M 1024
#define K 1024
#define REAL float 
#include "libxomp.h" 
#include "xomp_cuda_lib_inlined.cu" 
int i;
int j;
int k;
float a[1024UL][1024UL];
float b[1024UL][1024UL];
float c[1024UL][1024UL];
float c2[1024UL][1024UL];
int init();
int mmm();
int mmm2();
int verify();

int main()
{
  init();
  mmm();
  mmm2();
  return verify();
}

int init()
{
  for (i = 0; i < 1024; i++) 
    for (j = 0; j < 1024; j++) 
      a[i][j] = ((((3.0 * i) * j) / 1024) / 1024);
  for (i = 0; i < 1024; i++) 
    for (j = 0; j < 1024; j++) 
      b[i][j] = ((((5.0 * j) * i) / 1024) / 1024);
  for (i = 0; i < 1024; i++) 
    for (j = 0; j < 1024; j++) {
      c[i][j] = 0.0;
      c2[i][j] = 0.0;
    }
  return 0;
}
/*
TODO: try different i,j,k orders
a b     e f    a*e+ b*g , a*f+ b*h
c d  x  g h  = c*e+ d*g,  c*f+ d*h
*/

__global__ void OUT__1__11058__(float *_dev_a,float *_dev_b,float *_dev_c)
{
  int _p_i;
  int _p_j;
  int _p_k;
  int _dev_i = blockDim.x * blockIdx.x + threadIdx.x;
  if (_dev_i >= 0 && _dev_i <= 1023) {
    for (_p_j = 0; _p_j < 1024; _p_j++) 
      for (_p_k = 0; _p_k < 1024; _p_k++) 
        _dev_c[_dev_i * 1024UL + _p_j] = (_dev_c[_dev_i * 1024UL + _p_j] + (_dev_a[_dev_i * 1024UL + _p_k] * _dev_b[_p_k * 1024UL + _p_j]));
  }
}

int mmm()
{
{
    float *_dev_a;
    int _dev_a_size = sizeof(float ) * (N - 0) * (M - 0);
    _dev_a = ((float *)(xomp_deviceMalloc(_dev_a_size)));
    xomp_memcpyHostToDevice(((void *)_dev_a),((const void *)a),_dev_a_size);
    float *_dev_b;
    int _dev_b_size = sizeof(float ) * (M - 0) * (K - 0);
    _dev_b = ((float *)(xomp_deviceMalloc(_dev_b_size)));
    xomp_memcpyHostToDevice(((void *)_dev_b),((const void *)b),_dev_b_size);
    float *_dev_c;
    int _dev_c_size = sizeof(float ) * (N - 0) * (M - 0);
    _dev_c = ((float *)(xomp_deviceMalloc(_dev_c_size)));
    xomp_memcpyHostToDevice(((void *)_dev_c),((const void *)c),_dev_c_size);
/* Launch CUDA kernel ... */
    int _threads_per_block_ = xomp_get_maxThreadsPerBlock();
    int _num_blocks_ = xomp_get_max1DBlock(1023 - 0 + 1);
    OUT__1__11058__<<<_num_blocks_,_threads_per_block_>>>(_dev_a,_dev_b,_dev_c);
    xomp_freeDevice(_dev_a);
    xomp_freeDevice(_dev_b);
    xomp_memcpyDeviceToHost(((void *)c),((const void *)_dev_c),_dev_c_size);
    xomp_freeDevice(_dev_c);
  }
  return 0;
}

int mmm2()
{
  for (i = 0; i < 1024; i++) 
    for (j = 0; j < 1024; j++) 
      for (k = 0; k < 1024; k++) 
        c2[i][j] = (c2[i][j] + (a[i][k] * b[k][j]));
  return 0;
}

int verify()
{
  float sum = 0.0;
  float sum2 = 0.0;
  for (i = 0; i < 1024; i++) 
    for (j = 0; j < 1024; j++) {
      sum += c[i][j];
      sum2 += c2[i][j];
    }
  printf("sum of c[i][j] is %f\n",sum);
  printf("sum of c2[i][j] is %f\n",sum2);
  return 0;
}
