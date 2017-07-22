int main() {
  double x=10;
  int y;
  y=0;
  double a[2];
  a[y]=x+0.0;
  a[y]=x*1.0;
  a[y]=0.0*x;
  a[y]=x*1.0;
  a[y]=(x+0.0)*1.0;
  a[y]=(x*1.0)+0.0;
  a[y]=x+(x-x);
  return 0;
}
