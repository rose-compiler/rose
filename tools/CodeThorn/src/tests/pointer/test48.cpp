int main() {
  int a[]={1,2,3};
  int c;
  c=a[1];
  a[1]=10;
  a[2]=a[0];
  a[2]=a[a[2]];
  a[a[0]]=100;
  return 0;
}
