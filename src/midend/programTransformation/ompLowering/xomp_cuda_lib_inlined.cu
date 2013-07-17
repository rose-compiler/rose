/* 
CUDA and/or nvcc does not have linker for device code.
We have to put some common device functions into this file.
So the generated CUDA file can include the functions as inlined functions.

Liao 2/11/2013
*/

/*  reduction minus is handled the same way as reduction plus since we just replace the reduction variable with its local copy for each thread
    The associated statement is intact except for the variable replacement : e.g. a-=5 becomes local_a -= 5;
    in the end of each thread accumulates thread local negative values.
    At the block level, we just simply add them all to be the block level negative values
*/
/* we have to encode the type into function name since C function signature does not include parameter list! */
#define XOMP_INNER_BLOCK_REDUCTION_DEF(dtype) \
__device__ void xomp_inner_block_reduction_##dtype(dtype local_value, dtype * grid_level_results, int reduction_op) \
{ \
  /* __shared__ float* sdata[gridDim.x]; not compilable */ \
  /* block size of data, size is specified by the kernel launch parameter (3rd one) */ \
  /* shared data has to have different names for different types. Cannot reuse name across types. */ \
  extern __shared__ dtype sdata_##dtype[];  \
  sdata_##dtype[threadIdx.x] = local_value;  \
  __syncthreads(); \
  /* blockDim.x is the block size */ \
  int isEvenSize = (blockDim.x % 2 ==0); \
  /* contiguous range pattern: half folding and add */ \
  for(int offset = blockDim.x / 2; \
      offset > 0;    /* folding and add */ \
      offset >>= 1) /* offset shrinks half each time */ \
  { \
    if(threadIdx.x < offset)  \
    { \
      /* add a partial sum upstream to our own */ \
      switch (reduction_op){ \
        case XOMP_REDUCTION_PLUS: \
        case XOMP_REDUCTION_MINUS: \
            sdata_##dtype[threadIdx.x] += sdata_##dtype[threadIdx.x + offset]; \
            break; \
         /*  TODO add support for more operations*/ \
         default:  \
            { \
              /* TODO: add assertion or set cudaError with an error code */ \
              /* cannot call a host function */ \
              /* fprintf (stderr, "Error. xomp_inner_block_reduction() unhandled reduction operation:%d\n",reduction_op); */ \
              /* assert (false); */ \
             } \
      } /* end switch */ \
    } \
    /* remember to handle the left element */ \
    if ((threadIdx.x == 0) && !isEvenSize) \
    { \
      switch (reduction_op){ \
        case XOMP_REDUCTION_PLUS: \
        case XOMP_REDUCTION_MINUS: \
          sdata_##dtype[0]+= sdata_##dtype[2*offset];  \
          break; \
        /* TODO add more operation support */  \
        default: \
          {  \
            /* TODO: add assertion or set cudaError with an error code */  \
            /* cannot call a host function */ \
            /* fprintf (stderr, "Error. xomp_inner_block_reduction() unhandled reduction operation:%d\n",reduction_op); */ \
            /* assert (false); */ \
          } \
      } /* end switch */ \
    } \
    isEvenSize = ( offset % 2 ==0); /* prepare next round*/ \
    /* MUST wait until all threads in the block have updated their partial sums */ \
    __syncthreads(); /* sync after each folding */ \
  } \
  /* thread 0 writes the final result to the partial sum of this thread block */ \
  if(threadIdx.x == 0) \
  { \
    grid_level_results[blockIdx.x] = sdata_##dtype[0]; \
  } \
}

XOMP_INNER_BLOCK_REDUCTION_DEF(int)
XOMP_INNER_BLOCK_REDUCTION_DEF(float)
XOMP_INNER_BLOCK_REDUCTION_DEF(double)

#undef XOMP_INNER_BLOCK_REDUCTION_DEF 

/* Loop scheduling helper functions on GPUs */
//Default loop scheduling, worksharing without any schedule clause
// input upper bound is inclusive (loop normalized with <= or >=)
// output n_upper is also inclusive 
// stride is positive for incremental, negative for decremental iteration space
__device__ void XOMP_cuda_loop_default_internal(int lower, int upper, int stride, int _p_num_threads, int _p_thread_id, long* n_lower, long* n_upper)
{
  int _p_lower;
  int _p_upper;
  int _p_chunk_size;
  int addOne ; // adjustment to input and output upper bounds, depending on if they are inclusive or non-inclusive

  int isDecremental= 0;
  if (lower>upper)
    isDecremental = 1;

  // calculate loop iteration count from lower, upper and stride , 
  // adjust inclusive stride here TODO
  // no -1/+1? if upper is already an inclusive bound
  int _p_iter_count = 0;
  if (isDecremental == 1)
  {
    addOne = 1;
    //stride = 0 - stride;  // n
    if (stride >0)
    {
/*  TODO: assertion and error code  
      printf("Error: in XOMP_loop_default() of xomp.c: stride must be negative for decremental iteration. stride = %d \n ", stride);
      assert (0);
*/
    }
  }
  else // incremental
  {
    addOne = -1; // real bound should be non-inclusive - 1
    if (stride <0)
    {
/* TODO assertion and error code
      printf("Error: in XOMP_loop_default() of xomp.c: stride must be positive for incremental iteration. stride = %d \n ", stride);
      assert (0);
*/
    }
  }
  // addOne is needed here only if the input upper bound is non-inclusive
  // we use loop normalization to ensure upper bounds are inclusive already.
  // So we don't need addOne here anymore
  //_p_iter_count = ( stride + addOne + upper -  lower) /  stride;
  _p_iter_count = ( stride + upper -  lower) /  stride;

  // calculate a proper chunk size
  // two cases: evenly divisible  20/5 =4
  //   not evenly divisible 20/3= 6
  // Initial candidate  
 // get number of threads of this thread block, 1-D case
//  int _p_num_threads = blockDim.x ;  
  //int _p_num_threads = omp_get_num_threads();

  _p_chunk_size = _p_iter_count / _p_num_threads;
  int _p_ck_temp = _p_chunk_size * _p_num_threads != _p_iter_count;

  _p_chunk_size = _p_ck_temp + _p_chunk_size;

  // decide on the lower and upper bound for the current thread
  //int _p_thread_id = threadIdx.x ;
  //int _p_thread_id = omp_get_thread_num();
//  printf("inside xomp_loop_default(): _p_thread_id =%d\n", _p_thread_id);
  _p_lower =  lower + _p_chunk_size * _p_thread_id *  stride;

  //addOne is needed here if the output upper bound is inclusive
  // -1 if the output n_upper is an inclusive bound, 
  // we do use inclusive in the final normalized loop
  _p_upper = _p_lower + _p_chunk_size *  stride + addOne;

  // adjust inclusive stride here 
  // addOne is needed if the input upper bound is non-inclusive
  // no -1/+1 since upper is already an inclusive bound
  if (isDecremental == 1)
    _p_upper = (_p_upper > (upper ) ?_p_upper : (upper ));
    //_p_upper = (_p_upper > (upper + addOne) ?_p_upper : (upper + addOne));
  else
    _p_upper = (_p_upper < upper ?_p_upper : upper);
    //_p_upper = (_p_upper < (upper + addOne) ?_p_upper : (upper + addOne));

  *n_lower = _p_lower;
  *n_upper = _p_upper;
//  printf("inside xomp_loop_default(): _p_lower=%d, _p_upper=%d\n", _p_lower,_p_upper);
}

/* This is the top level function which decides on the lower and upper bounds for the current thread id and current block id
  It internally calls XOMP_cuda_loop_default_internal twice to figure this out: 
  once for the block's bounds, the other for the thread's bounds
*/
__device__ void XOMP_accelerator_loop_default(int lower, int upper, int stride, long* n_lower, long* n_upper)
{
/* Obtain the lower and upper bounds for the current 1-D thread block*/
 /*  the iteration bounds for this entire thread block */
  long lower_for_block, upper_for_block;
  XOMP_cuda_loop_default_internal (lower, upper, stride, gridDim.x, blockIdx.x, &lower_for_block, &upper_for_block);

 /* now focus on the bounds of the current thread of the current block */
  XOMP_cuda_loop_default_internal (lower_for_block, upper_for_block, stride, blockDim.x, threadIdx.x, n_lower, n_upper);
}


