#include<omp.h>
#include <stdio.h>

void paroutput(char* s)
{
#pragma omp parallel
printf("%s\n",s);
}

int main(void)
{
#pragma omp parallel
  {
    paroutput("before single");

#pragma omp single
    {
      paroutput("inside single");
    }

    paroutput("after single");
  }
  return 0;
}


