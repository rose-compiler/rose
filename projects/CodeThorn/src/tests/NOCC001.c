// testing equality/inqeuality of non-null pointers

#include <stdio.h>
#include <stdlib.h>

// 0 cnt cases of dead code (8 tests), printing of results contains on dead printf
int main() {
  int arbitrary=rand();
  int a,b,c,d;
  int* ap=&a;
  int* bp=&b;
  int* cp=&c;
  int* dp=&d;
  int* p1;
  int* p2;
  int* p3;
  struct S {
    int x;
    struct S* next;
  };
  int cnt=0;
  
#if 1
  if(arbitrary) {
    p1=ap;
  } else {
    p1=bp;
  }
  if(arbitrary) {
    p2=cp;
  } else {
    p2=dp;
  }

  if(ap==p1) {
    cnt++;
  }
  if(p1<p2) {
    cnt++;
  }
  if(p1>p2) {
    cnt++;
  }

  // p3 is undef, must be unknown
  if(ap==p3) {
    cnt++;
  }
  if(p1<p3) {
    cnt++;
  }
  if(p1>p3) {
    cnt++;
  }
  if(p1==p3) {
    cnt++;
  }
  if(p1!=p3) {
    cnt++;
  }
#endif
  
#if 0
  // precise analysis can determine this
  if(p1==p2) {
    cnt++;
  }
  // precise analysis can determine this
  if(p1!=p2) {
    cnt++;
  }
#endif

#if 0
  struct S s;
  s.next=&s;
  s.next->next=0;

  // true
  if(s.next==0) {
    printf("1\n");
    cnt++;
  }

  // false
  if(s.next!=0) {
    printf("2\n");
    cnt++;
  }
#endif

}
