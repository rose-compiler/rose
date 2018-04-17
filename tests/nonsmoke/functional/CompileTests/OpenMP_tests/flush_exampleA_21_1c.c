/*
 OpenMP spec30.pdf Example A.21.1c
*/
#include <omp.h>
#define NUMBER_OF_THREADS 256
int synch[NUMBER_OF_THREADS];
float work[NUMBER_OF_THREADS];
float result[NUMBER_OF_THREADS];
float fn1(int i)
{
  return i*2.0;
}

float fn2(float a, float b)
{
  return a + b;
}
int main()
{
  int iam, neighbor;
#pragma omp parallel private(iam,neighbor) shared(work,synch)
  {
    iam = omp_get_thread_num();
    synch[iam] = 0;
#pragma omp barrier
    /*Do computation into my portion of work array */
    work[iam] = fn1(iam);
    /* Announce that I am done with my work. The first flush
     * ensures that my work is made visible before synch.
     * The second flush ensures that synch is made visible.
     */
#pragma omp flush(work,synch)
    synch[iam] = 1;
#pragma omp flush(synch)
    /* Wait for neighbor. The first flush ensures that synch is read
     * from memory, rather than from the temporary view of memory.
     * The second flush ensures that work is read from memory, and
     * is done so after the while loop exits.
     */
    neighbor = (iam>0 ? iam : omp_get_num_threads()) - 1;
    while (synch[neighbor] == 0) {
#pragma omp flush(synch)
    }
#pragma omp flush(work,synch)
    /* Read neighbor’s values of work array */
    result[iam] = fn2(work[neighbor], work[iam]);
  }
  /* output result here */
  return 0;
}
