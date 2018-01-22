int main() {
  double x1=10.0,x2=20.0,x3=30.0,x4=40.0;
  int y;
  y=0;
  double a[2];
  a[y]=x2*x1;
  a[y]=(x2*x1)*(x4*x3);
  a[y]=x2+x1;
  a[y]=(x2+x1)+(x4+x3);
  a[y]=x3+x2+x1;
  a[y]=x3+(x2+x1);
  a[y]=x3*(x2+x1);
  a[y]=x3+(x2*x1);
  a[y]=(x2+x1)+x3;
  a[y]=(x3+(1.0+2.0))*(1.0+(x2+2.0));
  a[y]=(a[y]+(a[y]-1.0+2.0))*(1.0+(a[y]+a[y]+1.0));
  return 0;
}
