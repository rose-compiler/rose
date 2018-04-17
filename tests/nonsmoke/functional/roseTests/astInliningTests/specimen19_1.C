void foo(int x) {++x;}

int main(int, char**) {
  int w = 7;
  foo(w);
  return !(w == 7);
}
