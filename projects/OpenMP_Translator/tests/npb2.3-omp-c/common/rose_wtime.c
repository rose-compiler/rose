extern void *_pp_globalobject;
extern void *__ompc_lock_critical;
#include "wtime.h"
#include <sys/time.h>
#include "ompcLib.h" 

void wtime_(double *t)
{
  static int sec = (-1);
  struct timeval tv;
//  gettimeofday(&tv, (void *)0);
  gettimeofday(&tv,(((struct timezone *)0)));
  if (sec < 0) {
    sec = ((tv.tv_sec));
  }
  else {
  }
   *t = ((((tv.tv_sec) - (sec))) + (9.9999999999999995e-07 * ((tv.tv_usec))));
}

