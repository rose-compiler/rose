struct cuda_traversal {};


template <typename LOOP_BODY>
__global__ void forall_kernel(LOOP_BODY loop_body, int length)
{

  int ii = blockDim.x * blockIdx.x + threadIdx.x;
  if (ii < length) {
    loop_body(ii);
  }
}

template <typename LOOP_BODY>
void forall(cuda_traversal, int length, LOOP_BODY loop_body)
{
   size_t const blockSize = 256;
   size_t const gridSize = (length+blockSize-1) / blockSize ;

   forall_kernel<<<gridSize, blockSize>>>(loop_body, length);
}

