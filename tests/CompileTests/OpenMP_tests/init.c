// Test the placement of XOMP_init() in C/C++ input
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char* argv[])
{
  srand48;
  if (argc <2 )
    exit (1);
  int nc = 0;

#pragma omp parallel
#pragma omp master
  {
    printf("Number of threads = %d\n", omp_get_num_threads());
  }

  return 0;  
}
