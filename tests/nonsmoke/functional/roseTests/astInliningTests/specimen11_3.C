int foo(int x) {
  return x + 7;
}

int main(int, char**) {
  int w;
  w = foo(foo(foo(5)));
  return 0;
}
