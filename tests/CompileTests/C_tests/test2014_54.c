// Not that in old-style C, the arguments will be unparsed in a 
// different order, and this appears to be related to why "count"
// is swapped with "tm2" in the generated code.

#define DEMO_BUG 1

struct _proftime_T
   {
     int tv_usec;
     int tv_sec;
   };

typedef struct _proftime_T proftime_T;

void
profile_divide(tm, count, tm2)
#if DEMO_BUG
    proftime_T  *tm;
    proftime_T  *tm2;
    int		count;
#else
    proftime_T  *tm;
    int		count;
    proftime_T  *tm2;
#endif
   {
     if (count == 0)
        {
       // The bug is that this will be unparsed as:
       // double usec = ((tm -> tv_sec) * 1000000.0 + (tm -> tv_usec)) / tm2;
	       double usec = (tm->tv_sec * 1000000.0 + tm->tv_usec) / count;
        }
   }

