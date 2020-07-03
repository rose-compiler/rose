#include "DOM.h"
int main() {
  int a[2]={1,2};
  // arithmetic with array elements
  printf("a[0]:%d\n",a[0]);
  printf("a[1]:%d\n",a[1]);
  a[0]=a[0]+1;
  printf("a[0]:%d\n",a[0]);
  int* p;
  p=a;
  // dereference of pointers to arrays
  *p=*p+1;
  printf("*p:%d\n",*p);
  // pointer arithmetic (with pointers to named objects)
  p=p+1;
  *p=*p+1;
  printf("*p:%d\n",*p);
  int* q;
  // pointer artihmetic with pointers to arrays
  q=a+1;
  printf("*q:%d\n",*q);
  // 2-level pointers
  int** r;
  r=&p;
  printf("**r:%d\n",**r);
  // 2-level pointer dereferencing
  **r=**r+1;
  printf("**r:%d\n",**r);
  // compution on array elements referred by array index expressions
  int i=0;
  a[i+1]=a[i+1]+1;
  int* s;
  s=&a[1];
  int* b[2];
  int e1;
  b[0]=&a[1];
  b[1]=&e1;
  *b[1]=1;
  printf("a[0]:%d\n",a[0]);
  printf("a[1]:%d\n",a[1]);
  printf("*b[0]:%d\n",*b[0]);
  printf("*b[1]:%d\n",*b[1]);
  return 0;
}
