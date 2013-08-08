#include <stdio.h>
int main() {
  int sum=0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      sum=sum+j;
    }
  }
  printf("sum:%d\n",sum);
  return 0;
}
