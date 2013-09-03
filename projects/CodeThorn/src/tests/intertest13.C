int f2(int z) {
  return 10;
}

int f1(int f1_y) {
  int f1_x;
  f1_x=f1_y;
  return f2(f1_x);
}

int main() {
  int main_x;
  int y=1;
  main_x=f1(y);
  return 0;
}
