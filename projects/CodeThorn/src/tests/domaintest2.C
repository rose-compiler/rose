int d[3];
int e[]={10,11,12};

int main() {
  int a[3];
  int b[]={1,2,3,4};
  a[1]=b[2];
  a[2]=e[0];
  d[0]=a[2];
  int *p1;
  p1=a;
  int *p2; //=p1;
  p2=p1;
  int *p3=a;
  *p2=100;
  *p2=101;
  return 0;
}
