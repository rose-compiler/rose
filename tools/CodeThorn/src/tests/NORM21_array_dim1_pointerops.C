int main() {
  int a[]={1,2,3};
  int* p0;
  p0=a;
  int x;
  x=*p0;
  p0=&a[1];
  *p0=15;
  int* p1;
  p1=&x;
  *p1=20;
  int* p2;
  p2=p1;
  *p1=5000;
  return 0;
}
