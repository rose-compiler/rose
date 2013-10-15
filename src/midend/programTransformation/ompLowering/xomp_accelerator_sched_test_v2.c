// Liao 8/30/2013
// A dedicated self-contained file to test a scheduler using round-robin method across multiple threads 
// v1: using exclusive upper bounds
// V2: using inclusive upper bounds
//
// Compile : gcc -fopenmp thisfile.c

#include <stdio.h>
#include <omp.h>
#include <assert.h>

/*
_p_num_threads: number of threads of the thread team participating the scheduling
_p_thread_id: the current thread's id within the current team

Return the adjusted numbers including:
  loop_chunk_size: the real chunk size considering original chunksize and step
  loop_sched_index: the lower bound for current thread
  loop_stride: the total stride for one round of scheduling of all threads
*/
void XOMP_static_sched_init(int lb, int up, int step, int orig_chunk_size, int _p_num_threads, int _p_thread_id, \
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
int XOMP_static_sched_next(
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

void OUT__2__10550__(int n,int *_dev_u)
{
  int ij;
  int _dev_lower, _dev_upper;


  // variables for adjusted loop info considering both original chunk size and step(strip)
  int _dev_loop_chunk_size;
  int _dev_loop_sched_index;
  int _dev_loop_stride;

  // 1-D thread block:
  int _dev_thread_num = omp_get_num_threads();
  int _dev_thread_id = omp_get_thread_num();
  printf ("thread count = %d, current thread id = %d\n", _dev_thread_num, _dev_thread_id);

  int orig_start =0; // must be correct!!
  int orig_end = n-1; // TODO exclusive bound 
  int orig_step = 1;
  int orig_chunk_size = 1;

  XOMP_static_sched_init (orig_start, orig_end, orig_step, orig_chunk_size, _dev_thread_num, _dev_thread_id, \
      & _dev_loop_chunk_size , & _dev_loop_sched_index, & _dev_loop_stride);

  printf ("Initialized chunk size = %d, sched indx =%d, stride = %d\n",_dev_loop_chunk_size, _dev_loop_sched_index, _dev_loop_stride);

  while (XOMP_static_sched_next (&_dev_loop_sched_index, orig_end, orig_step, _dev_loop_stride, _dev_loop_chunk_size, _dev_thread_num, _dev_thread_id, & _dev_lower
        , & _dev_upper))
  {
    printf ("Thread ID: %d Allocated lower = %d upper = %d\n", _dev_thread_id, _dev_lower, _dev_upper);
    for (ij = _dev_lower ; ij <= _dev_upper; ij ++) { // using inclusive bound here
        _dev_u[ij] += (n - ij);         
    }
  }
}


#define SIZE 10
int a[SIZE], b[SIZE], c[SIZE] ;
int main ()
{
  int i;

// reference array and values for each element
  for (i=0; i<=SIZE-1; i++)
  { 
    a[i] = 0;
    b[i] = 0;
    c[i] = 0;
    a[i] += (SIZE - i); // reverse order to make sure no default values are messing up things
                      // futher using += to catch duplicated execution of one iteartion!
  }


// calcualted array elements using the scheduling functions
#pragma omp parallel sections num_threads(4)
  {
#pragma omp section
    OUT__2__10550__ (SIZE, b);
#pragma omp section
    OUT__2__10550__ (SIZE, b);
#pragma omp section
    OUT__2__10550__ (SIZE, b);
#pragma omp section
    OUT__2__10550__ (SIZE, b);
  }
printf  ("-------------------1 thread case ------------\n");
 // calcualted array elements using the scheduling functions
#pragma omp parallel sections num_threads(1)
  {
#pragma omp section
    OUT__2__10550__ (SIZE, c);
  }

 
  for (i=0; i<SIZE; i++)
  {
    printf ("a[%d]=%d, b[%d]=%d, c[%d]= %d \n", i, a[i], i, b[i], i, c[i]);
  }

  for (i=0; i<SIZE; i++)
  {
    assert (a[i]==b[i] && a[i]==c[i]);
  }
 printf ("Success if you see this printf output!\n");
 return 0;
}

