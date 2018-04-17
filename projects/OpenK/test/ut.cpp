#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct uint3 {int x, y, z;} uint3;
typedef struct dim3 {int x, y, z;} dim3;

uint3  threadIdx;
uint3  blockIdx;
dim3  blockDim;
dim3 gridDim;
int  warpSize;


 void GPU___global__square_array(int abcd,int bcda,int cdba,float *a, int N)
{
   float __shared__example[100];
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx<N) __shared__example[idx] =__shared__example[idx]*__shared__example[idx];
}


// main routine that executes on the host
int main(void)
{
  float *a_h, *a_d;  // Pointer to host & device arrays
  const int N = 10;  // Number of elements in arrays
  size_t size = N * sizeof(float);
  a_h = (float *)malloc(size);        // Allocate array on host
  a_d = (float *)malloc(size);
  // Initialize host array and copy it to CUDA device
  for (int i=0; i<N; i++) a_h[i] = (float)i;

  memcpy (a_d, a_h, size );
  // Do calculation on device:
  int block_size = 4;
  int n_blocks = N/block_size + (N%block_size == 0 ? 0:1);
  GPU___global__square_array  ( n_blocks, block_size ,-1,a_d, N);
  // Retrieve result from device and store it in host array
  memcpy(a_h, a_d, size);
  // Print results
  for (int i=0; i<N; i++) printf("%d %f\n", i, a_h[i]);
  // Cleanup
  free(a_h);

  free(a_d);
}
