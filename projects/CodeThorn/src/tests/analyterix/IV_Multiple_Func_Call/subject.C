
// called twice:
int getPlusFour(int value) {
  return value + 4;
}

// called once:
int getPlusThree(int value) {
  return value + 3;
}

int main() {
  int i = 4;
  i = getPlusThree(i);

  int k = 24;
  int m = 14;
  k = getPlusFour(k);
  m = getPlusFour(m);

  int z = 1;
}
