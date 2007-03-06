int f(int fx, int fy) {
  int fz;
  fz=fx+fy;
  return fz;
}

int main() {
  int x,y,z1,z2,z;
  x=5;
  y=10;
  z=0;
  z1=f(x,y);
  z=5;
  z2=f(x,z1);
  z=z1+z2;
  return 0;
}
