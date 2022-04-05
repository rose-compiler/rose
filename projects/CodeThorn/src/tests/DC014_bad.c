// testing equality/inqeuality of non-null pointers

#include <stdio.h>
#include <stdlib.h>

// 8 cases of dead code (+ printing results contains 1 dead line)

int main() {
  int arbitrary=rand();
  int a,b,c,d;
  int* ap=&a;
  int* bp=&b;
  int* cp=&c;
  int* dp=&d;
  int* p1;
  int* p2;
  printf("started.\n");
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

  int cnt=0;
  int cntTotal=8;
  
  if((p1<p2)&&(p1>p2)) {
    printf("p1<p2&&p1>p2\n");
    cnt++;
  }
  if(p1==p2) {
    printf("p1==p2\n");
    cnt++;
  }
  if(p1!=p1) {
    printf("p1!=p1\n");
    cnt++;
  }
  if(p1!=&a) {
    printf("p1!=&a\n");
    cnt++;
  }
  if(p2==&a) {
    printf("p2==&a\n");
    cnt++;
  }
  if(p1==&c) {
    printf("&p1==&c\n");
    cnt++;
  }
  if(&a==p2) {
    printf("&a==p2\n");
    cnt++;
  }
  if(p1==ap&&p1==bp) {
    printf("p1==ap&&p1==bp\n");
    cnt++;
  }
  if(cnt==0) {
    printf("All tests passed (%d)\n",cntTotal);
  } else {
    printf("%d tests of %d failed.\n",cnt,cntTotal);
  }
}
