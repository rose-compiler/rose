#include <cstdio>
main() {
  int a,b,i,j,n, *ap,*bp,**cp;
  int sum;
  scanf("%d",&n);
  cp=&bp;
  ap=&a;
  bp=ap; 
  *cp=&b;
  a=0;
  b=0;
  i=n;
  j=n;
  while (i>0) {
    *ap=*ap+1;
    i=i-1;
    j=i;
    while (j>0) {
      *bp=*bp+1;
      j=j-1;
    }
  }
  sum=*ap + *bp;
  printf("%d\n",sum);
}
