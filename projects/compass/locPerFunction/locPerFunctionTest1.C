
// if LocPerFunction.Size = 3
void pass() {
  int x;
  x = 5;
  x = 5;
  x = 5;
}

void fail() {
  int x;
  x = 5;
  x = 5;
  x = 5;
  x = 5;
  x = 5;
  x = 5;
  x = 5;
  x = 5;
  x = 5;
  x = 5;
}
