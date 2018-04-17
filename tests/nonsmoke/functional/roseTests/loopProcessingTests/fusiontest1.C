

main() {

 int x[30], i;

  for (i = 1; i <= 10; i += 1) {
    x[2 * i] = x[2 * i + 1] + 2;
  }
  for (i = 1; i <= 10; i += 1) {
    x[2 * i + 3] = x[2 * i] + i;
  }

}
