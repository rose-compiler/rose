#ifdef WITH_IO
#include "stdio.h"
#endif

#ifdef WITH_MALLOC
#include "stdlib.h"
#endif

/*
array : 1,0,0,0 [4]
m1    : 1,1,2,220,0,0,30
p=m1  : 1,1,2,220,0,0,30
*/
int main() {
  unsigned int array[4];
  long x;
  x=10;
  array[0]=1; // long value to char array
  unsigned char* m1;
#ifdef WITH_MALLOC
  m1=malloc(7);
#else
  unsigned char array2[7];
  m1=array2;
#endif
  m1[0]=array[0];
  m1[1]=255;
  m1[2]=30;
  unsigned char* p;
  p=m1;
  //p[6]=99; // out-of-bounds-access
  //  return 0;

  m1[6]=*p;
  //  m1[6]=m1[4]+*(p+2);
  p++;
  x=258; // char: 1,2
  //p[0]=(unsigned char)((x>>8)&0xff);
  //p[1]=(unsigned char)((x   )&0xff);
  p+=2;
  *p=220;
#ifdef WITH_IO
  printf("array: ");
  for(int i=0;i<3;i++) {
    printf("%d ",array[i]);
  }
  printf("\nm1   : ");
  for(int i=0;i<7;i++) {
    printf("%d ",m1[i]);
  }
  printf("\np-offset: %d\n",p-m1);
#endif

  return 0;
}
