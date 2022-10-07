int z;

void f1(int b) {
  int c;
  b=b+1;
  c=b;
  z=z+c;
}

int f2(int a) {
  a=a+1;
  f1(a);
  return a;
}

int main() {
  int x;
  int y;
  z=0;
  x=1;
  y=2;
  f1(x);
  x=0;
  f2(y);
  return 0;
}
