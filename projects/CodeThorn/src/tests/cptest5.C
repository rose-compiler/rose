int z3;
int z4;

int f(int a) {
  a=a+1;
  z3=2;
  z4++;
  return a;
}

int main() {
  int x=1;
  int y=2;
  int z1;
  int z2;
  z3=1;
  z4=1;
  if(true) {
    z1++;
    f(x);
  }
  else {
    y=f(y);
  }
  z1=x;
  z2=y;
  return 0;
}
