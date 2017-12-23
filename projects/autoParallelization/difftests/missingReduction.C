// A test case with a mistake: missing reduction 
#include <stdio.h>
#include <omp.h>
 
int main(int argc, char *argv[]) 
{
  int N = 20;
  int total ;
  int i,j;

#pragma omp parallel for
  for (j=0;j<N;j++) {
    for (i=0;i<N;i++) {
      total += i+j ;
    }
  }
  printf("%d\n", total) ;
  return 0;
}
