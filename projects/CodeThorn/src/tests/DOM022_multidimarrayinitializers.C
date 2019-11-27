// multi-dimensional array initializers
int main() {
  int a[2][2]={{1,2},{3,4}};
  int b[][2]={{1,2},{3,4}};
  b[0][0]=a[1][1];
  return 0;
}
