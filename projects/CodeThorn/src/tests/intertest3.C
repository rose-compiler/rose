int f1(int x) {
  return x;
}
int f2(int x) {
  x=6;
  return 2;
}

int main() {
  int x=1;
  f1(x);
  f2(x);
  return 0;
}
