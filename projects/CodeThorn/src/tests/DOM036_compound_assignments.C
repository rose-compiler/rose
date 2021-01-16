#include <cstdio>

int main() {
  int x=0;
  x+=2;
  x-=1;
  x*=4;
  x/=2;
  x%=16;
  
  int y=7;
  y&=0b11;
  y|=0b1000;
  y^=0b010101;

  int z=8;
  z<<=3;
  z>>=2;

  // x=2,y=30,z=16
  printf("x=%d,y=%d,z=%d\n",x,y,z);
  return 0;
    
}
