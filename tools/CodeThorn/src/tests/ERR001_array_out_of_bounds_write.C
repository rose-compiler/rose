
void f(int b[3][2]) {
  b[1][2]=5;
  b[2][0]=1; // out of bounds write
}

int main() {
  int a[][2]={{1,2},{3,4}};
  f(a);
  return 0;
}
