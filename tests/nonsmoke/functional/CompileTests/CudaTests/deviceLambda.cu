// compile with: nvcc --expt-extended-lambda -std c++11 -c deviceLambda.cu

#include <cuda.h>
#include <cuda_runtime.h>

template <class CudaFn>
__global__
void launcher(CudaFn fn)
{
  int idx = threadIdx.x + blockIdx.x * blockDim.x;

  fn(idx);
}

int main()
{
  static const int sz = 256;
  static const int bl = 32;

  double* matrix = nullptr;

  cudaMalloc(&matrix, sz*sz*sizeof(double));

  dim3   dimBlock(bl, bl);
  dim3   dimGrid((sz+bl-1)/bl, (sz+bl-1)/bl);

  // device lambda
  launcher<<<dimGrid, dimBlock>>>( [matrix] __device__ (int idx) -> void
                                   {
                                     matrix[idx] = 0;
                                   }
                                 );
  return 0;
}
