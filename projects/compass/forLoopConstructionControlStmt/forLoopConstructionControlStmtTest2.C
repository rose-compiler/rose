#include <stdlib.h>

int main()
{
  int *array = (int*)malloc( 100*sizeof(int) );
  for( int i = 0; i < 100; i++ ){ array[i] = 0; }

  free(array);
  return 0;
} //main()
