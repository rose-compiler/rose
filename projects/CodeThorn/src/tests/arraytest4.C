int main() {
  int a[]={1,2,3};
  int* p0;
  int* p1;
  p0=a;
  p1=a+1;
  *p0=10;
  //p0=&a[0];
  *p0=15;
  //p1=&a[1];
  *p1=20;
  return 0;
}
