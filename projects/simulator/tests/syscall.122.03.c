#include <errno.h>
#include <sys/utsname.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
int main(int ac, char **av)
{
	struct utsname *buf;

	if ((buf = (struct utsname *)malloc((size_t)
    sizeof(struct utsname))) == NULL)
    err(1,"malloc failed");

  int result = uname(buf);
  if( result != 0 )
    err(1,"uname failed");

	free(buf);
	buf = NULL;

	return 0;
}

