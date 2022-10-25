int g(int x) {
  return x+1;
}
int f() {
  int x=1;
  return g(x);
}
int h() {
  return (int)g(1);
}

int main() {
  f();
  return 0;
}
