
void fibonacci() {
  int i;
  int a[100];

  a[0] = 0;
  a[1] = 1;

  for (i = 2; i < 100; i++) {
    a[i] = a[i-2] + a[i-1];
  }
}

