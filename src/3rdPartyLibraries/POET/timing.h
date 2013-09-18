#ifndef TIMING_H
#define TIMING_H

#ifdef _MSC_VER
#include <winsock.h> /* struct timeval */
#else
#include <sys/time.h>
#endif

double GetWallTime(void); /*QY: return wall clock time*/

/*QY: register a timing variable*/
void register_timing(double* variable, const char* msg); 

/*QY: register a frequency variable*/
void register_freq(unsigned* variable, const char* msg); 

/* report result of all registered timing variables*/
void report_timing(); 

#endif
