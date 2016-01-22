struct cuda_traversal {};


template <typename LOOP_BODY>
__global__ void forall_kernel(LOOP_BODY loop_body, int length)
{

  int ii = blockDim.x * blockIdx.x + threadIdx.x;
  if (ii < length) {
    loop_body(ii);
  }
}

// This is what we want users to write. And we key off of the use of the __device__ used to annotate the loop body.
// This code could use the __attribute((__device__)) if the simpler __device__ keyword does not compile.
template <typename LOOP_BODY>
void forall(cuda_traversal, int length, LOOP_BODY __device__ loop_body)
{
   size_t const blockSize = 256;
   size_t const gridSize = (length+blockSize-1) / blockSize ;

   forall_kernel<<<gridSize, blockSize>>>(loop_body, length);
}

