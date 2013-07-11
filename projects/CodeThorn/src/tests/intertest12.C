int f2() {
  return 10;
}

int f1() {
  int f1_x=1;
  return f2();
}

int f3() {
  return 120;
}

int main() {
  int main_x;
  main_x=f1();
  f3();
  return 0;
}
