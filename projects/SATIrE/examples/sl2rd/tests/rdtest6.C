int f(int fx, int fy) {
  int fz;
  fz=fx+fy;
  return fz;
}

int g(int gx, int gy) {
  int gz;
  gz=gx*gy;
  f(gx,gz);
  return gz;
}

int main() {
  int x,y,r1,r2,r3,r;
  x=5;
  y=10;
  r1=f(x,y);
  x=3;
  r2=f(x,r1);
  x=7;
  r3=g(x,r2);
  r=r1+r2;
  return 0;
}
