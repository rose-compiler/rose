int f() {
  return 0;
}

int g() {
  int x=1;
  return x+x+x;
}

int main() {
  int x=1;
  int (*fp)();
  fp=f;
  (*f)();
  return 0;
}
