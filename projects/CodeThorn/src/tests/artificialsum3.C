#include <cstdio>
main() {
  int n;
  int sum;
  scanf("%d",&n);
  int a=0;
  int b=0;
  int i=n;
  int j=n;
  for(i=n-1;i>=0;i--) {
    a=a+1;
    for (j=i;j>0;j--) {
      b=b+1;
    }
  } 
  sum=a + b;
  printf("%d\n",sum);
}
