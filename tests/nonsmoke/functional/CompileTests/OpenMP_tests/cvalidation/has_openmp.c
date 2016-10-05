#include <stdio.h>
#include "omp.h"
#include "omp_testsuite.h"

int
check_has_openmp (FILE * logFile)
{
  int rvalue = 0;
#ifdef _OPENMP
  rvalue = 1;
#endif
  return rvalue;
}

int
crosscheck_has_openmp (FILE * logFile)
{
  int rvalue = 0;
#if 0
  rvalue = 1;
#endif
  return rvalue;
}
