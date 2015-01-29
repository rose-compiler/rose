#ifdef PRINT_RESULT
#include <cstdio>
#endif
int main() {

  int x=1;
  int y=2;
  int z=3;
  int* xp=&x;
  int* yp=&x;
  int* zp=&x;
  x=x+1;
  y=x+2;
  z=y+3;
  (*xp)*=2;
  (*yp)*=*zp;
  (*zp)*=*xp+x;
#ifdef PRINT_RESULT
  printf("%d %d %d\n",x,y,z);
#endif
  return (x==512 && y==4 && z==7);
}
