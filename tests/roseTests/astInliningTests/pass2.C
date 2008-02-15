int foo(int x) {
  return x + 3;
}

int main(int, char**) {
  int w = 7;
  w = foo(7);
  return 0;
}
