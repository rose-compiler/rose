
int f1(int* x, int*r) {};

int foo() {
  int index,i=0;
  while (i) {
    f1(&index,&i);
  }
  return 0;
}
