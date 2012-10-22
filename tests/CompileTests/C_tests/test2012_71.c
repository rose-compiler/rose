#include <sys/wait.h>

static void
child_cleanup(int signo __attribute__((unused)))
{
  wait(((void *)0));
}

