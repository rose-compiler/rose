
int main() {
  int* x;
  x = new int;
  int*& y = x;

  y++;
  // error invalid free
  delete x;

  return 0;
}
