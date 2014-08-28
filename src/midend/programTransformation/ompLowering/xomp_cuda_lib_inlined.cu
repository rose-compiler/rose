/* 
CUDA and/or nvcc does not have linker for device code.
We have to put some common device functions into this file.
So the generated CUDA file can include the functions as inlined functions.

TODO: extend to support 3-D mapping case, which should be trivial based on 2-D implementation

Modified: 
*Liao, 7/10/2013, extend reduction support for 2-D mapping

Liao 2/11/2013
*/

/*  reduction minus is handled the same way as reduction plus since we just replace the reduction variable with its local copy for each thread
    The associated statement is intact except for the variable replacement : e.g. a-=5 becomes local_a -= 5;
    in the end of each thread accumulates thread local negative values.
    At the block level, we just simply add them all to be the block level negative values
*/
/* we have to encode the type into function name since C function signature does not include parameter list! */
/* grid_level_results: across blocks, each block has a result inside this array of size = gridDim.x * gridDim.y */
#define XOMP_INNER_BLOCK_REDUCTION_DEF(dtype) \
__device__ void xomp_inner_block_reduction_##dtype(dtype local_value, dtype * grid_level_results, int reduction_op) \
{ \
  /* To speedup reduction, we transfer local_value to a shared data within the block */ \
  /* __shared__ float* sdata[blockDim.x * blockDim.y * blockDim.z]; not compilable */ \
  /* block size of data, size is specified by the kernel launch parameter (3rd one) */ \
  /* shared data has to have different names for different types. Cannot reuse name across types. */ \
  extern __shared__ dtype sdata_##dtype[];  \
  /* map 2-D threads into a 1-D shared data: linearization */ \
  int ii = threadIdx.x*blockDim.y + threadIdx.y; \
  sdata_##dtype[ii] = local_value;  \
  __syncthreads(); \
  /* nn: the block size, number of threads per block */ \
  int nn = blockDim.x* blockDim.y ; \
  int isEvenSize = (nn % 2 ==0); \
  /* contiguous range pattern: half folding and add */ \
  for(int offset = nn / 2; \
      offset > 0;    /* folding and add */ \
      offset >>= 1) /* offset shrinks half each time */ \
  { \
    if(ii < offset)  \
    { \
      /* add a partial sum upstream to our own */ \
      switch (reduction_op){ \
        case XOMP_REDUCTION_PLUS: \
        case XOMP_REDUCTION_MINUS: \
            sdata_##dtype[ii] += sdata_##dtype[ii + offset]; \
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
    if ((ii == 0) && !isEvenSize) \
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
  /* thread (0,0,0) writes the partial sum of this thread block to grid level results (linearized also)*/ \
  if(ii == 0) \
  { \
    grid_level_results[blockIdx.x*gridDim.y + blockIdx.y] = sdata_##dtype[0]; \
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
// Updated on 8/29/2013 Liao
// It turns out that evenly dividing up iteration space for GPU threads is not optimal.
// Each thread may touch a large range (chunk) of data elements , which has low memory access efficiency (cannot be coalesced)
// A better solution is to use round -robin scheduling, similar to the static scheduling with chunk size 1 in regular OpenMP CPU threads loops
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

/*
_p_num_threads: number of threads of the thread team participating the scheduling
_p_thread_id: the current thread's id within the current team

  lb and up are inclusive bounds (after normalization)
Return the adjusted numbers including:
  loop_chunk_size: the real chunk size considering original chunksize and step
  loop_sched_index: the lower bound for current thread
  loop_stride: the total stride for one round of scheduling of all threads
*/
__device__ void XOMP_static_sched_init(int lb, int up, int step, int orig_chunk_size, int _p_num_threads, int _p_thread_id, \
              int * loop_chunk_size, int * loop_sched_index, int * loop_stride)
{   
    int nthds = _p_num_threads;

    if (nthds == 1) { // single thread case
      *loop_sched_index = lb;
      //loop_end = up;
      *loop_chunk_size = orig_chunk_size * step;
      *loop_stride = (*loop_chunk_size) * nthds;
      return;
    }

    *loop_chunk_size = orig_chunk_size * step;
    *loop_sched_index = lb + (*loop_chunk_size)* _p_thread_id;
    *loop_stride = (*loop_chunk_size) * nthds;
    //int loop_end = up;
//    int is_last = 0;
}

/*
Using current thread ID (_p_thread_id) and team size (_p_num_threads), calculate lb and ub for the current thread
for the round robin scheduling with lower (loop_sched_index), upper (loop_end) , stride (loop_stride), and chunk size (loop_chunk_size)
*/
__device__ int XOMP_static_sched_next(
    int* loop_sched_index , int loop_end, int orig_step, int loop_stride, int loop_chunk_size,
    int _p_num_threads, int _p_thread_id,
    int *lb,int *ub)
{   
    int b,e;
    b = *loop_sched_index;
  //The code logic is original for exclusive upper bound!!
  // But in ROSE, we normalize all loops to be inclusive bounds. So we have to ajust them in the functions, instead of during transformation.
  //
  // 1. adjust the original loop end from inclusive to be exclusive. 
    if (orig_step >0)
       loop_end ++; // expect the user code will use the upper bound as an inclusive one, so minus one in advance
    else
       loop_end --;

    if (_p_num_threads == 1) { /* not in parallel */
        e = loop_end;
        if(b == e) return 0;
        *lb = b;
        *ub = e;
        *loop_sched_index = e;
#if 1 // need to adjust here!
    if (orig_step >0)
       *ub --; // expect the user code will use the upper bound as an inclusive one, so minus one in advance
    else
       *ub ++;
#endif
        return 1;
    } // thread team has 1 thread only

    *loop_sched_index += loop_stride;

    e = b + loop_chunk_size;
#if 1 // must timely adjust e here !!
    if (orig_step >0)
       e --; // expect the user code will use the upper bound as an inclusive one, so minus one in advance
    else
       e ++;
#endif

    if(loop_chunk_size > 0){
        if(b >= loop_end) return 0;
        if(e >= loop_end){
            e = loop_end;
//            tp->is_last = 1;
        }
    } else {

        if(b <= loop_end) return 0;
#if 0 // too late to adjust, e is already used before!!
        if(e <= tp->loop_end){
            e = tp->loop_end;
            tp->is_last = 1;
        }
#endif
    }
    *lb = b;
    *ub = e;
   return 1;
}

// A wrapper function for  blockDim.x * blockIdx.x + threadIdx.x
// Essentially we just hide CUDA variables (blockDim.x etc) inside this function
// since there are three dimensions x, y, z. we use dimension_no to indicate which dimension is requested.
// dimension_no start from 1 to 3, corresponding to x, y, z dimensions.
__device__ int getLoopIndexFromCUDAVariables(int dimension_no)
{
  if (dimension_no == 1)
   return blockDim.x * blockIdx.x + threadIdx.x;
  else if (dimension_no == 2)
   return blockDim.y * blockIdx.y + threadIdx.y;
  else if (dimension_no == 3)
   return blockDim.z * blockIdx.z + threadIdx.z;
  else
  {
    //printf("getLoopIndexFromCUDAVariables() accept a parameter of range from 1 to 3 only\n");
    //assert (false);
  }
   return -1; 
}

// A wrapper function for gridDim.x * blockDim.x, to hide CUDA variables gridDim.x and blockDim.x.
__device__ int getCUDABlockThreadCount(int dimension_no)
{
   if (dimension_no == 1)
   return gridDim.x * blockDim.x;
  else if (dimension_no == 2)
   return gridDim.y * blockDim.y;
  else if (dimension_no == 3)
   return gridDim.z * blockDim.z;
  else
  {
    //printf("getCUDABlockThreadCount() accept a parameter of range from 1 to 3 only\n");
    //assert (false);
  }
   return -1; 
}


