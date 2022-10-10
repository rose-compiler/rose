// pointers to multidimensional arrays and their elements

#include <cstdlib>

int main() {

  int *ap;
  ap=(int*)malloc(3*sizeof(int));

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

  *ap=c[1][1][1];
  *((c[1][1])+1)=1000;

  //free(ap); seg-fault
  return 0;
}
