/* Liao, 5/16/2009
   Test macro expansion for __FILE__:
   For most *nix systems, it should be expanded to a file name without path

   SPEC CPU 2006' sphinx3 relies on this for execution verification
   EDG 3.3/4.0's macro.c has been changed to support this.
 */
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main()
{
  char * filename=__FILE__;
#ifdef __linux__
  printf("Linux is found, assertion is enabled... \n");  
  assert(strcmp(filename,"test2009_17.c")==0);
#else
  printf("Linux is not found, assertion is skipped. \n");  
#endif  
  return 0;
}

