void foo() {

  int x = 2;
  for (int i = 0; i < 4; i++) {
    x = 4;
    if (x == 4) {
      continue;
    }
    x = 5;
    if (x == 3) {
      break;
    }
  }
}
