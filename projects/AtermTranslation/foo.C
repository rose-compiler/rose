typedef int a, *b, &c, * const &d;

int main(int, char**) {
  int x = 3;
  return x + 5;
}
