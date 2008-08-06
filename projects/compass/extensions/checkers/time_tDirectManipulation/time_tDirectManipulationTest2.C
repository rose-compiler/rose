#include <time.h>

int do_work(int seconds_to_work) {
  time_t start;
  time_t current;
  start = time(NULL);
  current = start;

  if (start == (time_t)(-1)) {
    /* Handle error */
  }
  while (difftime(current, start) < seconds_to_work) {
    current = time(NULL);
    if (current == (time_t)(-1)) {
       /* Handle error */
    }
    //do_some_work();
  }

  return 0;
}
