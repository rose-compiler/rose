#include <cstdio>
main() {
  int n;
  int sum;
  scanf("%d",&n);
  int a=0;
  int b=0;
  int i=n;
  int j=n;
  while (i>0) {
    a=a+1;
    i=i-1;
    j=i;
    while (j>0) {
      b=b+1;
      j=j-1;
    }
  }
  sum=a + b;
  printf("%d\n",sum);
}
