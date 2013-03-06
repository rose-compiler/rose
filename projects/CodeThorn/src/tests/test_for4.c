#include <stdio.h>
// we intentionally avoid using ++,-- in this test
int main() {
  int sum=0;
  for(int i=1;i<3;i++) {
	for(int j=1;j<3;j++) {
	  sum=sum+j;
	}
  }
  printf("sum:%d\n",sum);
  return 0;
}
