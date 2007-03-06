int f(int fx, int fy) {
  int fz;
  fz=fx+fy;
  return fz;
}

int g(int gx, int gy) {
  int gz;
  gz=gx+gy;
  return gz;
}

int main() {
  int x,y,z1,z2,z;
  x=5;
  y=10;
  z1=f(x,y);
  z2=g(x,z1);
  z=z1+z2;
  return z-z;
}
