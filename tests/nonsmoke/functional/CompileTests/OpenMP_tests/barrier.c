#include<stdio.h> 

void do_sth()
{
  printf ("hello.\n");  
}
int main(void)
{
#pragma omp parallel
  {
    do_sth();
#pragma omp barrier
    do_sth();
  }
  return 0;
}
