// pointers to multidimensional arrays and their elements

int main() {

  int a[3];
  int *ap=a;

  int b[3][3];
  int *bp=b[1];
  int (*bq)[3]=b;

  int c[3][3][3];
  int *cp=c[2][2];
  int (*cq)[3]=c[1];

  ap=bp;
  bp=cp;
  bq=cq;
  *cp=500;
  *bp=*bp+1;
  *bp=*bp+1;

  *((c[1][1])+1)=1000;
  return 0;
}
