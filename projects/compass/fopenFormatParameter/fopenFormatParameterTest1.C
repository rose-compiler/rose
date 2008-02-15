#include <stdio.h>

int main()
{
  FILE *f = fopen( "/tmp/tmp.txt", "wr" );

  fclose( f );

  return 0;
}
