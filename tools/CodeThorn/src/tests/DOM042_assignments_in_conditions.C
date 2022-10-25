#include <cstdio>

int main() {
  int a=1;
  int b=1;
  int *p=&a;
  (*p)+=10;
  if((*p)>0) {
    b=b+10;
  }
  printf("a=%d,b=%d,*p=%d\n",a,b,*p);
  if(((*p)=100) > 0) {
    b=b+100;
  }
  printf("a=%d,b=%d,*p=%d\n",a,b,*p);
  if(((*p)+=1000) > 0) {
    b=b+1000;
  }
  printf("a=%d,b=%d,*p=%d\n",a,b,*p);
}

