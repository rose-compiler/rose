#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

/* Timer code (gettimeofday). */
double polybench_t_start, polybench_t_end;

static
inline
double rtclock()
{
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0)
      printf("Error return from gettimeofday: %d", stat);
    return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

inline
void polybench_timer_start()
{
  
  polybench_t_start = rtclock();
}

inline
void polybench_timer_stop()
{
    polybench_t_end = rtclock();
}

inline
void polybench_timer_print()
{
    printf("%0.6lfs\n", polybench_t_end - polybench_t_start);
}
