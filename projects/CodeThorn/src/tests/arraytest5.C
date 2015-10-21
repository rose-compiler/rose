int x;
int y;

int main() {

  int a[]={1,2,3};
  int b[3]={4,5,6};
  int c[5]={7,8,9};
  int* p;
  int y=0;
  p=a;
  x=a[0];
  x=b[1];
  x=x+c[2];
  x=c[y];
  if(c[y]) { y=y+1; }
  x=x+1;
  x=p[y];
  if(p[y]) { y=y+1;}
  return 0;
}
