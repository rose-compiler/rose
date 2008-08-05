#include <stdio.h>

int main()
{
  int a = 2;
  int b = 2;
  int c = 2;
  
  if ( a < b < c ) // condition #1, misleading, likely bug
    printf( "a < b < c\n" );
  if ( a == b == c ) // condition #2, misleading, likely bug
    printf( "a == b == c\n" );

  if ( a < b && b < c ) // clearer, and probably what was intended
    printf( "a < b && b < c\n" );
  if ( a == b && a == c ) // ditto
    printf( "a == b && a == c\n" );

  return 0;
}
