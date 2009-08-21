// Test handling of reference types
// Liao, 8/14/2009
#include <stdio.h>
int main(void)
{
  int j=10;
  int &i=j;
#pragma rose_outline
  {
    i=i+1;
    printf("Hello,world! I am %d\n",i);
  }

  printf("Hello,world! I am %d\n",i);
  return 0;
}

