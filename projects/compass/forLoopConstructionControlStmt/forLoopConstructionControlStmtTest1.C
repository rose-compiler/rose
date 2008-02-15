#include <stdlib.h>

int main()
{
  int *array = (int*)malloc( 100*sizeof(int) );
  for( int i = 0; i < 100; i++, array[i] = i ){}

  free(array);
  return 0;
} //main()
