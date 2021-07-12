int main() {
  int a[10000];
  struct S { int x; int y; };
  struct S b[10000];
  for(int i=0;i<10000;i++) {
    a[i]=1;
    b[i].x=10;
    b[i].y=20;
  }
}
