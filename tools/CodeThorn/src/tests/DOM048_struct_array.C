#include <stdio.h>

int main() {
  struct S {
    int x;
    int y;
  };
  int m=3;
  struct S b[3];
  // array index
  for(int i=0;i<m;i++) {
    b[i].x=i;
    b[i].y=i*2;
  }
  for(int i=0;i<m;i++) {
    printf("a[%d].x=%d\n",i,b[i].x); // read
    printf("a[%d].y=%d\n",i,b[i].y); // read
  }
  return 0;
}
