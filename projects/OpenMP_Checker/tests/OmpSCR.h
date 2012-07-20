/*
* OpenMP Source Code Repository
*
* Common funtions and tools header file
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _OMPSCR_H
#define _OMPSCR_H

/* SET SOME STUBS FOR SEQUENTIAL COMPILATION */
#if defined(_OPENMP)
#include <omp.h>
#else
#define omp_get_max_threads()  1
#define omp_get_thread_num()   0
#define omp_set_num_threads(x)
#define omp_get_num_threads() 1
#endif

/* MEMORY ALLOCATION WITH AUTOMATIC CHECK OF SUCCESS */
extern void *OSCR_calloc(size_t nmemb, size_t size);

/* INFORMATION TOOLS */
extern void OSCR_error(const char *mensaje, int errInfo);

/* INITIALIZATION */
void OSCR_init(
	int	numThreads,
	char *	description,
	char *	usage,
	int	numArgs,
	char *	argNames[],
	char *	defaultValues[],
	int	numTimers,
	int	numReportedTimers,
	char *	reportedTimerNames[],
	int	argc,
	char *	argv[]
	);

/* GETTING ARGUMENT VALUES */
extern int OSCR_getarg_int(int ind);
extern double OSCR_getarg_double(int ind);
extern char * OSCR_getarg_string(int ind);

/* STANDARD TIMER TOOLS */
extern void OSCR_timer_clear(int);
extern void OSCR_timer_start(int);
extern void OSCR_timer_stop(int);
extern double OSCR_timer_read(int);

/* STANDARD REPORT TOOL */
extern void OSCR_report();

#ifdef __USE_PAPI
#include <papi.h>
#include <papiStdEventDefs.h>

extern void init_PAPI(int numTimers);
extern void start_EVENTPAPI(int ind, int EventCode);
extern long_long stop_PAPI(int ind);
#endif

#endif
/*
* END _OMPSCR_H
*/
