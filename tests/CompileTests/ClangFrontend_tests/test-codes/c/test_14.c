
int factoriel_0(int n) {
  if (n > 1) return n * factoriel_0(n-1);
  else return 1;
}

int factoriel_1(int n) {
  return n > 1 ? n * factoriel_1(n-1) : 1;
}

int factoriel_2(int n) { 
  int res = n;
  if (n > 1) {
    res *= factoriel_2(n-1);
  }
  return res;
}

