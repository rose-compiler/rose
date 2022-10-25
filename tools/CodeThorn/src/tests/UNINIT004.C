#include <cstdlib>
#include <cstdio>

int main() {
  int * data;
  //data = (int *)malloc(10*sizeof(int));
  data = (int *)malloc(10);
  if (data == NULL) {
    return 1;
  }
  printf("%d\n",data[0]);
#if 0
  int i;
  for(i=0; i<10; i++) {
    printf("%d\n",data[i]);
  }
#endif
  return 0;
}
