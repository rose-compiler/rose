#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

char *TCID = "syscall.114";
int TST_TOTAL = 1;

int main() {
  pid_t pid = 0;
  int status;
  struct rusage usage;
  wait4(pid, &status, WNOWAIT, &usage);
  // TODO: Assert on return value
  return 0;
}
