#include <cstdio>
main() {
  int n;
  int sum;
  scanf("%d",&n);
  int a=0;
  int b=0;
  for(int i=n-1;i>=0;i--) {
    a=a+1;
    for (int j=i;j>0;j--) {
      b=b+1;
    }
  } 
  sum=a + b;
  printf("%d\n",sum);
}
