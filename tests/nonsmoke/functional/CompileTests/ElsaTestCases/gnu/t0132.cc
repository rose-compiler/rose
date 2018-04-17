// check __alignof__
int main() {
  struct foo {int x; int y;};
  int x = __alignof__(struct foo);
};
