int foo() {
  int a = 0;
  while (a < 5) {++a;}
  return a + 3;
}

int main(int, char**) {
  for (int x = foo(); false;) {}
  return 0;
}
