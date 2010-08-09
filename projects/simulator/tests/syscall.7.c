#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
  pid_t pid = 0;
  int status;
  waitpid(pid, &status, WNOWAIT);
  // TODO: Assert on return value
  exit(0);
}
