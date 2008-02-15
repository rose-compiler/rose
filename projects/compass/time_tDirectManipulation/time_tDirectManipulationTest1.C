#include <time.h>

int do_work(int seconds_to_work) 
{
  time_t start;
  start = time( NULL );

  if (start == (time_t)(-1)) {
    /* Handle error */
  }
  while (time(NULL) < start + seconds_to_work) 
  {
    //do_some_work();
  }

  return 0;
}
