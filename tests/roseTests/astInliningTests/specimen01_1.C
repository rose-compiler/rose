int foo() {
  return 5;
}

int main(int, char**) {
  int w;
  w = foo();
  return !(w == 5);
}
