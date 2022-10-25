int f(int x) {
  x=x+1;
  return x;
}

int main() {
  int a;
  int b;
  a=1;
  f(f(a));
  b=f(f(a)+1);
  return 0;
}
