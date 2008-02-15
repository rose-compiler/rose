#include <stdio.h>

int main()
{
  FILE *f = fopen( "/tmp/tmp.txt", "r" );

  fclose( f );

  return 0;
}
