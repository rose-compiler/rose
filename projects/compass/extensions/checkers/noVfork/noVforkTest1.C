#include <stdlib.h>
#include <unistd.h>

int main()
{
  pid_t pid = vfork();

  if ( pid == 0 )  /* child */ 
  {
    system( "echo \"Hello World\"" );
  }

  return 0;
}
