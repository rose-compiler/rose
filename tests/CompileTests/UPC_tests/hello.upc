#include <stdio.h>
int main(){
  printf("Hello World from thread %d of %d threads\n",\
	MYTHREAD,THREADS);
  upc_barrier;
  return 0;
}
