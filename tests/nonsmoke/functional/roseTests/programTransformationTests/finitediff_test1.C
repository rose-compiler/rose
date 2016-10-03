int foo(int n) {
  int y;
  for (int x = 0; x < 1000; ++x) {
    y = n * x;
  }
  return y;
}
