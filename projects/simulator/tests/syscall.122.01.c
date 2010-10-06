#include <sys/utsname.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <err.h>

int exp_enos[] = { 0, 0 };

struct utsname un;

int main() {
  int result = uname(&un);

  if( result == -1 )
    err(1,"uname failed");

	return 0;
}
