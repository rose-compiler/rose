
#include <stdio.h>
#include <assert.h>

#include <omp.h>
#include "omp_testsuite.h"

typedef struct
{
  char *name;
  a_ptr_to_test_function pass;
  a_ptr_to_test_function fail;
}
testcall;

static FILE *logFile;
static const char *logFileName = "test.log";

static testcall alltests[] = {

//#include "feasible_test.h"

{"has_openmp",check_has_openmp,crosscheck_has_openmp},
{"omp_nested",check_omp_nested,crosscheck_omp_nested},
{"omp_get_num_threads",check_omp_get_num_threads,crosscheck_omp_get_num_threads},
{"omp_in_parallel",check_omp_in_parallel,crosscheck_omp_in_parallel},
{"for_ordered",check_for_ordered,crosscheck_for_ordered},
{"for_reduction",check_for_reduction,crosscheck_for_reduction},
{"for_private",check_for_private,crosscheck_for_private},
{"for_firstprivate",check_for_firstprivate,crosscheck_for_firstprivate},
{"for_lastprivate",check_for_lastprivate,crosscheck_for_lastprivate},
{"section_reduction",check_section_reduction,crosscheck_section_reduction},
{"section_private",check_section_private,crosscheck_section_private},
{"section_firstprivate",check_section_firstprivate,crosscheck_section_firstprivate},
{"section_lastprivate",check_section_lastprivate,crosscheck_section_lastprivate},
{"single",check_single,crosscheck_single},
{"single_private",check_single_private,crosscheck_single_private},
{"single_nowait",check_single_nowait,crosscheck_single_nowait},
{"parallel_for_ordered",check_parallel_for_ordered,crosscheck_parallel_for_ordered},
{"parallel_for_reduction",check_parallel_for_reduction,crosscheck_parallel_for_reduction},
{"parallel_for_private",check_parallel_for_private,crosscheck_parallel_for_private},
{"parallel_for_firstprivate",check_parallel_for_firstprivate,crosscheck_parallel_for_firstprivate},
{"parallel_for_lastprivate",check_parallel_for_lastprivate,crosscheck_parallel_for_lastprivate},
{"parallel_section_reduction",check_parallel_section_reduction,crosscheck_parallel_section_reduction},
{"parallel_section_private",check_parallel_section_private,crosscheck_parallel_section_private},
{"parallel_section_firstprivate",check_parallel_section_firstprivate,crosscheck_parallel_section_firstprivate},
{"parallel_section_lastprivate",check_parallel_section_lastprivate,crosscheck_parallel_section_lastprivate},
{"omp_master_thread",check_omp_master_thread,crosscheck_omp_master_thread},
{"omp_critical",check_omp_critical,crosscheck_omp_critical},
{"omp_atomic",check_omp_atomic,crosscheck_omp_atomic},
{"omp_barrier",check_omp_barrier,crosscheck_omp_barrier},
{"omp_flush",check_omp_flush,crosscheck_omp_flush},
{"omp_threadprivate",check_omp_threadprivate,crosscheck_omp_threadprivate},
{"omp_copyin",check_omp_copyin,crosscheck_omp_copyin},
{"omp_lock",check_omp_lock,crosscheck_omp_lock},
{"omp_testlock",check_omp_testlock,crosscheck_omp_testlock},
{"omp_nest_lock",check_omp_nest_lock,crosscheck_omp_nest_lock},
{"omp_nest_testlock",check_omp_nest_testlock,crosscheck_omp_nest_testlock},
{"omp_num_threads",check_omp_num_threads,crosscheck_omp_num_threads},
{"omp_time",check_omp_time,crosscheck_omp_time},
{"omp_ticks_time",check_omp_ticks_time,crosscheck_omp_ticks_time},
{"single_copyprivate",check_single_copyprivate,crosscheck_single_copyprivate},
{"for_schedule_static",check_for_schedule_static,crosscheck_for_schedule_static},
{"for_schedule_dynamic",check_for_schedule_dynamic,crosscheck_for_schedule_dynamic},
{"for_schedule_guided",check_for_schedule_guided,crosscheck_for_schedule_guided},

  {"end", 0, 0}
};

int
main (int argc, char **argv)
{
  int i = 0;
  int failed = 0;
  int success = 0;
  int crosschecked = 0;
  int N = 20;
  int j = 0;
  int result = 1;
  int crossfailed = 0;


  logFile = fopen (logFileName, "a");

  printf ("######## OpenMP Validation Suite V 1.0 ######\n");
  printf ("## Repetitions: %3d                       ####\n", N);
  printf ("## Loop Count : %6d                    ####\n", LOOPCOUNT);
  printf ("##############################################\n");


  while (alltests[i].pass)
    {
      crossfailed = 0;
      result = 1;
      fprintf (logFile,
               "--------------------------------------------------\n%s\n--------------------------------------------------\n",
	       alltests[i].name);
      printf ("%s ... ", alltests[i].name);
      fflush (stdout);
      for (j = 0; j < N; j++)
	{
	  fprintf (logFile, "# Check: ");

	  if (alltests[i].pass (logFile))
	    {
	      fprintf (logFile, "No errors occured during the %d. test.\n",
                       j + 1);
              if (!alltests[i].fail (logFile))
		{
		  fprintf (logFile, "# Crosscheck: Verified result\n");
		  crossfailed++;
		}
	      else
		{
		  fprintf (logFile, "# Crosscheck: Coudn't verify result.\n");
		}
	    }
	  else
	    {
	      fprintf (logFile, "--> Erroros occured during the %d. test.\n",
                       j + 1);
              result = 0;
	    }
	}
      if (result == 0)
	{
	  failed++;
	}
      else
	{
	  success++;
	}
      if (crossfailed > 0)
	{
	  crosschecked++;
	}
      fprintf (logFile, "Result for %s:\n", alltests[i].name);
      if (result)
	{
	  fprintf (logFile,
                   "Directiv worked without errors.\nCrosschecks verified this result with %5.2f%% certainty.\n",
		   100.0 * crossfailed / N);
          printf (" verified with %5.2f%% certainty\n",
		  100.0 * crossfailed / N);
	}
      else
	{
	  fprintf (logFile, "Directive failed the tests!\n");
	  printf (" FAILED\n");
	}


      i++;
    }
  printf
    ("\n\n Performed a total of %d tests, %d failed and %d successful with %d cross checked\nFor detailled inforamtion on the tests see the logfile: (%s).\n",
     i, failed, success, crosschecked, logFileName);
  printf ("Also check failed.compilation for tests which did not pass the compilation.\n");
  fprintf (logFile,
           "\n\n Performed a total of %d tests, %d failed and %d successful with %d cross checked\n",
	   i, failed, success, crosschecked);
  fclose (logFile);
  return failed;
}
