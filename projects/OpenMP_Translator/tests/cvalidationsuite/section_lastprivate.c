#include <stdio.h>
#include "omp_testsuite.h"

int
check_section_lastprivate (FILE * logFile)
{
  int sum = 0;
  int sum0 = 0;
  int known_sum;
  int i;
  int i0 = -1;
#pragma omp parallel
  {
#pragma omp sections  lastprivate(i0) private(i,sum0)
    {
#pragma omp section
      {
	sum0 = 0;
	for (i = 1; i < 400; i++)
	  {
	    sum0 = sum0 + i;
	    i0 = i;
	  }
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }				/* end of section */
#pragma omp section
      {
	sum0 = 0;
	for (i = 400; i < 700; i++)
	  {
	    sum0 = sum0 + i;	/*end of for */
	    i0 = i;
	  }
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
#pragma omp section
      {
	sum0 = 0;
	for (i = 700; i < 1000; i++)
	  {
	    sum0 = sum0 + i;
	    i0 = i;
	  }
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
    }				/* end of sections */
  }				/* end of parallel */
  known_sum = (999 * 1000) / 2;
  return ((known_sum == sum) && (i0 == 999));
}

int
crosscheck_section_lastprivate (FILE * logFile)
{
  int sum = 0;
  int sum0 = 0;
  int known_sum;
  int i;
  int i0 = -1;
#pragma omp parallel
  {
#pragma omp sections  private(i0) private(i,sum0)
    {
#pragma omp section
      {
	sum0 = 0;
	for (i = 1; i < 400; i++)
	  {
	    sum0 = sum0 + i;
	    i0 = i;
	  }
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }				/* end of section */
#pragma omp section
      {
	sum0 = 0;
	for (i = 400; i < 700; i++)
	  {
	    sum0 = sum0 + i;	/*end of for */
	    i0 = i;
	  }
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
#pragma omp section
      {
	sum0 = 0;
	for (i = 700; i < 1000; i++)
	  {
	    sum0 = sum0 + i;
	    i0 = i;
	  }
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
    }				/* end of sections */
  }				/* end of parallel */
  known_sum = (999 * 1000) / 2;
  return ((known_sum == sum) && (i0 == 999));
}
