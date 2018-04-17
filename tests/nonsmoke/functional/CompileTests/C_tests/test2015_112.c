// switch_bug.cpp: bug in switch-case construct

#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv)
{
  int i = 1;
  switch(i) {
     static int s = 42;
     s = 0;      // unreachable code
  case 1:
     i = s;
     break;
  case 2:
     i = 2;
     break;
  default:
     i = 99;
  }
  printf("i = %d (%s)\n", i, (i==42)?"correct":"incorrect!");
  assert (i == 42);
  return 0;
}

