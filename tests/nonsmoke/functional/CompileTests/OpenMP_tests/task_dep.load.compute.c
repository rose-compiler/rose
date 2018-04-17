// Test a two-stage pipeline parallelism using OpenMP 4.0's task depend clause
//
//  for () {load (A[i]);  compute(A[i]); }
//
//  Serial execution timeline:
//
//  load a[0] -> compute a[0] -> load a[1] -> compute a[2] -> load a[3] -> compute a[3]
//
//  Pipelined execution : each load X, compute Y is formed in a task.
//  Only keep necessary dependences between load a[i]-> compute a[j] when i==j  ):
//  Otherwise, all tasks are executed in parallel. 
//  
//  Execution Time line: 
//  load a[0] -> compute a[0] 
//   load a[1] -> compute a[1] 
//    load a[2] -> compute a[2] 
//
//  Performance difference: 
//   serial execution: 50*2= 100 seconds
//   pipelinined execution: 4 seconds. 
//
// Tested using
// * Intel icc 16.0.210: pass
// * clang 3.9: pass
// * gcc 4.9.3: pass
//
//  Liao, 2/21/2017
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <omp.h>

#define MYLEN 50
int a[MYLEN];

//dummy memory operation: load a[i] and initialize it. 
void load(int i)
{
//  sleep(1);
 int tid = omp_get_thread_num();
 printf ("thread %d loads %d\n", tid, i);
  a[i]= i;  
}

// dummy computation operation on a[i]
void compute(int i)
{
//  sleep(1);
 int tid = omp_get_thread_num();
 printf ("thread %d computes %d\n", tid, i);
  a[i]= a[i]+1;  
}

void verify ()
{
  int i;
  for (i=0; i<MYLEN; i++)
  {
    if (a[i]!= i+1)
    {
      printf("error: a[%d] = %d, not expected %d\n", i, a[i], i+1);
    }
    assert (a[i]==i+1);
  }
}

int main()
{
  int i=0; 
#pragma omp parallel 
#pragma omp single
  {
    for (i=0; i<MYLEN; i++)
    { 
// MUST USE firstprivate(i) here. Otherwise it inherits the shared attribute from enclosing region.
//#pragma omp task depend(out:i) firstprivate(i) // wrong, must dep on a[i] to have distinct dep pairs.
#pragma omp task depend(out:a[i]) firstprivate(i)
      load (i);
#pragma omp task depend(in:a[i]) firstprivate(i)
      compute(i);
    }
  }
  // implicit barrier in the end of omp parallel. All tasks must finish here.
  // So we can verify results then. 

  verify();
  return 0; 
}

