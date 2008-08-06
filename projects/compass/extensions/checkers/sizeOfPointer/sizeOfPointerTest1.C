
//Your test file code goes here.



#include <stdio.h>

int main ()
{
  
  int y = 7;
  int *x = &y;
  int **t = &x;
  int ***z = &t;
  y =  sizeof(y);
  y =  sizeof(x);
  y =  sizeof(*x);
  y =  sizeof(t);
  y =  sizeof(*t);
  y =  sizeof(**t);
  y =  sizeof(z);
  y =  sizeof(*z);
  y =  sizeof(**z);
  y =  sizeof(***z);
  return 0;
}
