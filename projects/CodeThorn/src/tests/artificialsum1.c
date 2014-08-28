#include <cstdio>
main() {
  int a,b,i,j,n;
  int sum;
  scanf("%d",&n);
  a=0;
  b=0;
  i=n;
  j=n;
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
