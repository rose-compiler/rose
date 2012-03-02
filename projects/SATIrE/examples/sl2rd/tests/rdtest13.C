int f(int fx, int fy) {
  int fz;
  fz=fx+fy;
  return fz;
}

int g(int gx, int gy) {
  return gx*f(gx,gy);
}

int main() {
  int x,y,r1,r2,r;
  x=5;
  y=10;
  r1=f(x,y);
  x=3;
  r2=f(x,r1)+g(x,r1);
  r=r1+r2;
  return 0;
}
