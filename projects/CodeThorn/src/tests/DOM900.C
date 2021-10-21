
#ifdef DOM_STD_INCLUDES
#include <cstdio>
#else
int printf(const char*,int);
int printf(const char*,int*);
int printf(const char*,int(*)[3]);
#endif

int main() {

  int a[3]={100,101,102};
  int* p1=a;
  
  printf("a:%p\n",a);
  printf("p1:%p\n",p1);
  printf("p1==a:%d\n",p1==a);

  int* p2=&a[0];
  printf("p2:%p\n",p2);
  printf("p1==p2:%d\n",p1==p2);  

  int* p3=&a[1];
  p3=p3; // corner case
  printf("p3:%p\n",p3);
  printf("(p1+1)==p3:%d\n",(p1+1)==p3);  

  printf("*p1:%d\n",*p1);
  printf("*p2:%d\n",*p2);
  printf("*p3:%d\n",*p3);
  printf("(p1+1)==p3:%d\n",(p1+1)==p3);  

#ifdef C99_ARRAY_POINTERS
  // C99
  int (*p4) [3];
  p4=&a;
  printf("p4:%p\n",p4);
  printf("p1==p4:%d\n",p1==(int*)p4); // true
  printf("*p4:%p\n",*p4);
  printf("**p4:%d\n",**p4);
  p4++; // increments by size of entire array
  int (*p5) [3];
  p5=p4;
  p5=p5+2;
  printf("p5:%p\n",p5);
  printf("p1==p5:%d\n",p1==(int*)p5); // false
  printf("*p5:%p\n",*p5);
  printf("p5-p4:%ld\n",p5-p4); // 2
  printf("sizeof(a):%ld\n",sizeof(a)); // 12
  
  int* x;
  x=*p4;
#endif
  
  return 0;
}

// codethorn --interpreter-mode=1 --context-sensitive --in-state-string-literals --exploration-mode=topologic-sort --log-level=none --normalize-level=2 --abstraction-mode=0 --array-abstraction-index=-1 --tg1-memory-subgraphs=yes tests/DOM900.C

