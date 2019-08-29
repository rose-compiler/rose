// compile with: nvcc --expt-extended-lambda -std c++11 -c hostDdeviceLambda.cu

#include <cuda.h>
#include <cuda_runtime.h>

template <class CudaFn>
__global__
void launcher(CudaFn fn)
{
  int idx = threadIdx.x + blockIdx.x * blockDim.x;

  fn(idx);
}

template <class Fn>
void run(int sz, Fn fn)
{
  // run large matrices on GPU
  if (sz*sz >= 256)
  {
    static const int bl = 32;

    dim3   dimBlock(bl, bl);
    dim3   dimGrid((sz+bl-1)/bl, (sz+bl-1)/bl);

    launcher<<<dimGrid, dimBlock>>>(fn);
    return;
  }

  // and small ones sequentially
  for (int i = 0; i < sz*sz; ++i)
    fn(i);
}

int main()
{
  static const int sz = 256;

  double* matrix = nullptr;

  cudaMallocManaged(&matrix, sz*sz*sizeof(double));

  // host device lambda
  run( sz, [matrix] __host__ __device__ (int idx) -> void
       {
         matrix[idx] = 0;
       }
     );
  return 0;
}
