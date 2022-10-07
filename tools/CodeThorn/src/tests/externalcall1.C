int f(int x, int y);

int g(int a, int b) {
  return a+b;
}

int main() {
  int mx=1,my=1;
  int ma=2,mb=2;
  f(mx,my);
  g(ma,mb);
  f(mx+10,my+10);
  g(ma+10,mb+10);
}
