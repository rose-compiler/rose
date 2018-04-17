int func(int i) {
  if(i == 0) {
    return 0;
  }
  int res;
  res = func(i - 1);
  res = res + i;
  return res;
}

int main() {
  func(5);
}
