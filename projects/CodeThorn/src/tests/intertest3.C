int f1(int x3) {
  return x3;
}
int f2(int x2) {
  x2=6;
  return 2;
}

int main() {
  int x=1;
  f1(x);
  f2(x);
  return 0;
}
