#include <stdio.h>
int main(int argc, char** argv){
  printf("Hello World! From thread %d ", MYTHREAD);
  printf(" of total %d threads\n",THREADS);
  upc_barrier;
  return 0;
}
