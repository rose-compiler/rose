int f2(int f2_x) {
  int f2_y=f2_x;
  return f2_y;
}

int f1(int f1_y) {
  int f1_x;
  f1_x=f1_y;
  return 1+f2(f1_x);
}

int main() {
  int main_x;
  int y=100;
  main_x=f1(y);
  return 0;
}
