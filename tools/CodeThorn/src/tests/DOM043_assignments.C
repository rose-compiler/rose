#include <cstdio>

int main() {
  int x=1;
  x=x+1;
  x++;
  printf("1 x=%d\n",x);
  for(int y=0;y<5;y++) {
    x=x=1;
  }
  printf("2 x=%d\n",x);
  for(int y=0;(x++,y<5);y++) {
  }
  printf("3 x=%d\n",x);
  for(int y=x++;y<10;y++) {
  }
  x=3;
  for(;x--;) {
    printf("4 x=%d\n",x);
  }
  printf("5 x=%d\n",x);
  x=3;
  while(x--) {
    printf("6 x=%d\n",x);
  }
}
