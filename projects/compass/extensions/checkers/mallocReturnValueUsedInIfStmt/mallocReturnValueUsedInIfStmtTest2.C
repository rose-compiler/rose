#include <stdlib.h>

int *foo()
{
  return (int*)malloc( 256*sizeof(int) );
}

int main()
{
  int *iptr = (int*)malloc( 256*sizeof(int) );
  int *iptr2 = NULL;

  {
    iptr2 = (int*)malloc( 256*sizeof(int) );
  }

  if( iptr2 == NULL ){}

  return 0;
} //main()
