#include <stdio.h>
#include <stdlib.h>

static int INSERT_HERE;

int main(int argc,char *argv[])
{
  int x = 5;
  double y = 20.5;
  
  INSERT_HERE = 0;
  y = y * x + 1.11111;
  return (int )y;
}

void woah()
{
  int a = 1;
  a++;
}
