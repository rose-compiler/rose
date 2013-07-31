#include <stdio.h>

int main() {
  int input; 
  int x=0;
  int y=0;
  scanf("%d",&input);
  if(input==1) {
    if(input==1) {
      y=1;
    } else {
      y=2;
    }
    if(input!=1) {
      y=11;
    } else {
      y=12;
    }
  } else {
    if(input==1) {
      y=101;
    } else {
      y=102;
    }
    if(input!=1) {
      y=1001;
    } else {
      y=1002;
    }
  }
  printf("%d",y);
  return 0;
}

