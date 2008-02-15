int foo(int x, int y) {
#pragma SliceTarget
  int z = x + 1;
  return z;
}

#pragma SliceFunction
void main() {

  int i = 0;
  int q = 0;
  int j = 2;
  while (q < 6) {
    i = foo(i, j);
    i = i + 1;
    q = q + 1;
    j = j + 1;
  }
  i;
}


