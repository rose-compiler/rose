#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void drop_priv()
{
  struct passwd *passwd;

  if ((passwd = getpwuid(getuid())) == NULL)
  {
    printf("getpwuid() failed");
    return;
  }
  printf("Drop user %s's privilege\n", passwd->pw_name);
  seteuid(getuid());
}

int main(int argc, char *argv[])
{
  drop_priv();
  printf("About to exec\n");
  execv(argv[1], argv + 1);
}
