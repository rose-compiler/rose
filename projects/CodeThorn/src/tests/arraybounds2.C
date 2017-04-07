#ifdef WITH_IO
#include "stdio.h"
#endif

#ifdef WITH_MALLOC
#include "stdlib.h"
#endif

/*
array : 10,_,_ [3]
m1    : 10,512,30,_,_,... [20]
p=m1  :  33,0,10,255,0
*/
int main() {
  unsigned int array[3];
  long x;
  x=10;
  array[0]=1; // long value to char array
  unsigned char* m1;
#ifdef WITH_MALLOC
  m1=malloc(5);
#else
  unsigned char array2[5];
  m1=array2;
#endif
  m1[0]=array[0];
  m1[1]=255;
  m1[2]=30;
  unsigned char* p;
  p=m1;
  //p[6]=99; // out-of-bounds-access
  return 0;

  *p=33;
  p++;
  x=258; // char: 1,2
  p[0]=(unsigned char)((x>>8)&0xff);
  p[1]=(unsigned char)((x   )&0xff);
  p+=2;
  *p=220;
#ifdef WITH_IO
  printf("array: ");
  for(int i=0;i<3;i++) {
    printf("%d ",array[i]);
  }
  printf("\nm1   : ");
  for(int i=0;i<5;i++) {
    printf("%d ",m1[i]);
  }
  printf("\np-offset: %d\n",p-m1);
#endif

  return 0;
}
