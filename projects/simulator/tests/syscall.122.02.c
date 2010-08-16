#include <errno.h>
#include <sys/utsname.h>

#include <err.h>

int main(int ac, char **av)
{
  int result = uname((struct utsname *)-1);

  if( result != -1 )
    errx(1,"uname succeeded unexectedly");
  if( errno != EFAULT )
    err(1,"Expected EFAULT");
  return 0;
}
