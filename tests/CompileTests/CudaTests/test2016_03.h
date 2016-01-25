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
// Note that the simpler __device__ keyword will compile, but I can't locate the information in the EDG AST.
// I think there is not mechanism for storing it because it is a part of the storage modifier, and function
// parameters don't have a storage modifier.  so EDG might have to be extended to capture this information
// and store it for use by ROSE.  The alternative implemtnation was to use the GNU attribute mechanism
// and this works (though since this is not a supported GNU attribute the implementation (ROSE) had be
// be extended to support it.  Within ROSE the data is place into the function parameter, the IR had to 
// be extended to support this.
template <typename LOOP_BODY>
// void forall(cuda_traversal, int length, LOOP_BODY __device__ loop_body)
void forall(cuda_traversal, int length, LOOP_BODY __attribute((device)) loop_body)
{
   size_t const blockSize = 256;
   size_t const gridSize = (length+blockSize-1) / blockSize ;

   forall_kernel<<<gridSize, blockSize>>>(loop_body, length);
}

