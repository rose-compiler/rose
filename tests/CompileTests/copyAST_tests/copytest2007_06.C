// #include <time.h>

#define __SLONGWORD_TYPE	long int

#define __TIME_T_TYPE		__SLONGWORD_TYPE

#define __STD_TYPE __extension__ typedef
__STD_TYPE __TIME_T_TYPE __time_t;	/* Seconds since the Epoch.  */


int x;
