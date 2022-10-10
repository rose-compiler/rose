int f() {
  return 0;
}

int g() {
  return 0;
}

int main() {
  int x=1;
  int (*fp)();
  fp=f;
  (*f)();
  return 0;
}
