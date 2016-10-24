int foo() {
  int a = 0;
  while (a < 5) {++a;}
  return a + 3;
}

int main(int, char**) {
  for (; foo() < 7;) {return 0;}
  return 0;
}
