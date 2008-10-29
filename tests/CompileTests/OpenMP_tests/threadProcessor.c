#include <stdio.h>
#include <omp.h>
#include <pthread.h>
extern int pthread_num_processors_np(void);
int main(void)
{
int tid,procid;
omp_set_num_threads(4);

#pragma omp parallel private(tid,procid)
{
  tid=omp_get_thread_num();
  procid=pthread_num_processors_np();
  printf("Hello,world.! by thread %d  on processor %d\n",tid,procid);

}
}

