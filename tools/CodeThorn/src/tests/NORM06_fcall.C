int f() {
  return 1;
}

int main() {
  int y=0;
  int a[2];
  a[0]=a[1]=a[2]=0;
  a[y]=a[f()];
  a[y]=a[a[a[y+1+f()]]];
  return 0;
}
