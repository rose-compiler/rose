#include <cuda.h>
#include <cuda_runtime.h>

__global__
void
__launch_bounds__(128, 2)
init(double *x)
{
  int idx = threadIdx.x + blockIdx.x * blockDim.x;

  x[idx] = 0;
}

int main()
{
  static const int sz = 256;
  static const int bl = 32;

  double* matrix = nullptr;

  cudaMalloc(&matrix, sz*sz*sizeof(double));

  dim3   dimBlock(bl, bl);
  dim3   dimGrid((sz+bl-1)/bl, (sz+bl-1)/bl);
  init<<<dimGrid, dimBlock>>>(matrix);

  return 0;
}
