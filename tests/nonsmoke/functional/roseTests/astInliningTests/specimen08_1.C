int foo() {
  int a = 0;
  while (a < 5) {++a;}
  return a + 3;
}

int main(int, char**) {
  int x = 0;
  for (; x < 7; x = foo()) {}
  return 0;
}
