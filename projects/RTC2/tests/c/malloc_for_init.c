
#include <stdlib.h>


int main()
{
  for (int* x = (int*)malloc(8*sizeof(int)), y = x+8; x < y; ++x)
  {

  }
}
