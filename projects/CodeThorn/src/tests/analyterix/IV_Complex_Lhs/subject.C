int& forward(int& value) {
  return value;
}

int main() {
  int i = 4;
  int m = 3;

  m = 6;

  (i, m) = 5; // <=> m = 5

  if(m != 5) {
    "unreachable";
  }

  (i < 10 ? i : m) = 7; // <=> i = 7

  if(i != 7) {
    "unreachable2";
  }

  int k = 2;

  forward(k) = 9; // <=> k = 9

  if(k != 9) {
    "unreachable3";
  }

  int y = 0;
}
