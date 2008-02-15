int fact(int n) {
  if (n == 0)
    return 1;
  else
    return n * fact(n - 1);
}

int main(int, char**) {
  return !(fact(5) == 120);
}
