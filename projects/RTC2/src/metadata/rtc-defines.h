
#ifndef RTC_DEFINES_H
#define RTC_DEFINES_H 1

#define SILENT_ERRORS 0      /* log errors to file */
#define DESCRIPTIVE_ERRORS 0 /* prints error msg before failure */
#define THREADX_DEBUG 0      /* log exectution to file */

#define INT_ARRAY_INDEX 0    /* uses int for bounds checking (check lower bound) */

#define USE_DUMMY_LOCK 1        /* switches dummy locks */
#define DUMMY_LOCK 50           /* ?? */
#define DUMMY_KEY 0             /* ?? */

#define USE_DUMMY_BOUNDS 1      /* switches dummy bounds */
#define DUMMY_LOWER_BOUND 11000 /* ?? */
#define DUMMY_UPPER_BOUND 11000 /* ?? */

#define STACK_DEBUG 0           /* print debug messages for arg passing */
#define DEBUG 0                 /* switches debug msgs */

#define TIMING_SUPPORT 0        /* adds timing */
#define GET_STATS 1             /* collects runtime stats */

#endif /* RTC_DEFINES_H */
