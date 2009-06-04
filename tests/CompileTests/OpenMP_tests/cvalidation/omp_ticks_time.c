#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<unistd.h>


#include "omp_testsuite.h"
#include "omp_my_sleep.h"


/*int omp_check_ticks_time(FILE * logFile)
*/
int
check_omp_ticks_time (FILE * logFile)
{
  double tick;
  tick = omp_get_wtick ();
  fprintf (logFile, "work took %f sec. time. \n", tick);
  return (tick > 0.0) && (tick < 0.01);
}

/*int omp_crosscheck_ticks_time(FILE * logFile)
*/
int
crosscheck_omp_ticks_time (FILE * logFile)
{
  double tick = 0;
  /*tick=omp_get_wtick(); */
  fprintf (logFile, "work took %f sec. time. \n", tick);
  return (tick > 0.0) && (tick < 0.01);
}
