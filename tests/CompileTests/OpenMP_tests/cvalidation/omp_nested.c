/*
Test if the compiler support nested parallelism
By Chunhua Liao, University of Houston
 Oct. 2005
*/
#include <stdio.h>
#include "omp.h"
#include "omp_testsuite.h"

int check_omp_nested( FILE *logFile)
{
int counter =0 ;

#ifdef _OPENMP
   omp_set_nested(1);
#endif

#pragma omp parallel shared(counter)
{
#pragma omp critical
  counter ++;
#pragma omp parallel
 {
#pragma omp critical
  counter --;
 }
}
  return (counter!=0);
}

int crosscheck_omp_nested( FILE *logFile)
{
int counter =0 ;
#ifdef _OPENMP
   omp_set_nested(0);
#endif

#pragma omp parallel shared(counter)
{
#pragma omp critical
  counter ++;
  #pragma omp parallel
 {
  #pragma omp critical
  counter --;
 }
}
  return (counter!=0);
}

