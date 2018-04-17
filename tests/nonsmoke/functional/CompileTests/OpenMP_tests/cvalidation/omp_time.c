#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<unistd.h>


#include "omp_testsuite.h"
#include "omp_my_sleep.h"

/*int omp_check_time(FILE * logFile)
*/
int
check_omp_time (FILE * logFile)
{
  double start = 0;
  double end = 0;
  int wait_time = 1;
  double measured_time;
  start = omp_get_wtime ();
  my_sleep (wait_time);
  end = omp_get_wtime ();
  measured_time = end - start;
  fprintf (logFile, "work took %f sec. time. \n", measured_time);
  return (measured_time > 0.9 * wait_time)
    && (measured_time < 1.1 * wait_time);
}

/*int omp_crosscheck_time(FILE * logFile)
*/
int
crosscheck_omp_time (FILE * logFile)
{
  double start = 0;
  double end = 0;
  int wait_time = 1;
  double measured_time;
  /*start=omp_get_wtime(); */
  my_sleep (wait_time);
  /*end=omp_get_wtime(); */
  measured_time = end - start;
  fprintf (logFile, "work took %f sec. time. \n", measured_time);
  return (measured_time > 0.9 * wait_time)
    && (measured_time < 1.1 * wait_time);
}
