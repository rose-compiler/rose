// 4 Matches

int main() {
  int *a = new int, *b = new int;
  *a = 5;
  *b = 7;
  if (a == b) {}
  if (a > b) {}
  if (a < b) {}
  if (a >= b) {}
  if (a <= b) {}
  delete a;
  delete b;
  return 0;
}
