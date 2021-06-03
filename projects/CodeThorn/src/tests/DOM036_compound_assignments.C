#include <cstdio>

int g(int b) {
  return b+1;
}

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

  int f=1;
  f=g(f+=1);
  int* p=&f;
  (*p)+=10;
  struct S { int u; };
  S s;
  s.u=1;
  printf("s.u=%d\n",s.u);
  s.u+=1;
  printf("s.u=%d\n",s.u);
  p=&s.u;
  (*p)+=1;
  printf("*p=%d\n",*p);
  (*p)++;
  printf("*p=%d\n",*p);
  s.u++;
  printf("s.u=%d\n",s.u);
  (*p)+=1;
  (*p)+=2000;
  if((*p)>0) {
    f=f+100;
  }
  int t=s.u;
  printf("x=%d,y=%d,z=%d,f=%d,t=%d\n",x,y,z,f,t);
  return 0;
    
}
