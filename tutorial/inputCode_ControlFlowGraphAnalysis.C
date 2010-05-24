#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[])
{
  size_t i;
  char buffer[10];
  for (i=0; i < strlen(argv[1]); i++)
  {
 // Buffer overflow for strings of over 9 characters
    assert(i < 10);
    buffer[i] = argv[1][i];
  }
  return 0;
}

int testIf(int i)
{
  int rt; 
  if (i%2 ==0)
    rt =0;
  else 
    rt =1;

  return rt;
}
