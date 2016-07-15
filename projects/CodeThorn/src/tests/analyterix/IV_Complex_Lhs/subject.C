int& forward(int& value) {
  return value;
}

int main() {
  int i = 4;
  int m = 3;

  m = 6;

  (i, m) = 5; // <=> m = 5

  (i < 10 ? i : m) = 7; // <=> i = 7

  int k = 2;

  forward(k) = 9; // <=> k = 9

  int y = 0;
}
