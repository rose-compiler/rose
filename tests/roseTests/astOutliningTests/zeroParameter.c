/*
*/
#include <stdio.h>
float x;
int y;

int main (int argc, char * argv[])
{
    x=1.0;
    y=1;
#pragma rose_outline
  {
 printf("x=%f, y=%d\n",x,y);
  }

  return 0;
}


