int foo(int x) {
  return x + 3;
}

int bar(int y) {
  return foo(y) + foo(2);
}

int main(int, char**) {
  int w;
  w = bar(1);
  return 0;
}
