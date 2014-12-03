// 2 Matches

int gcd(int a, int b) {
  while (a && b) {
    (a > b)? a %= b : b %= a;
  }
  return (a>b)?a:b;
}

int main() {
  return 0;
}
