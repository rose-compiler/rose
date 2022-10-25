int f2() {
  return 1;
}

int f1() {
  int f1_x=2;
  int f1_r;
  f1_r=f2();
  return f1_r;
}

int f3() {
  return 120;
}

int main() {
  int main_x;
  main_x=f1();
  return 0;
}
