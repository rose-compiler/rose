#include <stdlib.h>
int main(int argc, char* argv[])
{   
  int i;
  int len = 100;

  if (argc>1)
    len = atoi(argv[1]);

  int a[len];

  for (i=0; i<len; i++)
    a[i]= i; 

#pragma omp parallel for
  for (i=0;i< len -1 ;i++)
    a[i]=a[i+1]+1;

  return 0;
}

