void f2() {
  int x=0;
  if (x==0)
    x=2;
}

void f1() {
  int x;
  x = 3;
  int y = 3;
  int z = x + y;
  f2();
}

int main() {
  f1();
  f2();
  return 0;
}
