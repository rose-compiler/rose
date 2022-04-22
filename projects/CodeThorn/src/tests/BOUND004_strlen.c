#include <stdlib.h>

int main() {
  char target[3];
  int x=0;
  x=strlen("abc");
  printf("%d\n",x);
  if(x>=3) {
    x=100;
  }
  printf("%d\n",x);
}
