
//Your test file code goes here.
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SLEEP_TIME 3

volatile sig_atomic_t sig1 = 0;
volatile sig_atomic_t sig2 = 0;

void handler(int signum) {
  if (sig1) {
     sig2 = 1;
  }
  if (signum == SIGUSR1) {
    sig1 = 1;
  }
}

int main(void) {
  signal(SIGUSR1, handler);
  signal(SIGUSR2, handler);

  while (1) {
    if (sig2) break;
    sleep(SLEEP_TIME);
  }

  /* ... */

  return 0;
}
