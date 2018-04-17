int main() {
  double x=10;
  int y;
  y=0;
  double a[2];
  a[y]=x+0.0;
  a[y]=x*1.0;
  a[y]=x*0.0; // not to be changed
  a[y]=x+1.0; // not to be changed
  a[y]=x*1.0;
  a[y]=(x+0.0)*1.0;
  a[y]=(x*1.0)+0.0;
  a[y]=0.0+x;
  a[y]=1.0*x;
  a[y]=0.0*x; // not to be changed
  a[y]=1.0+x; // not to be changed
  a[y]=1.0*x;
  a[y]=1.0*(x+0.0);
  a[y]=0.0+(x*1.0);
  return 0;
}
