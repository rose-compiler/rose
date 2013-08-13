int x;

void f2();

void f2() {
  x=12;
  return;
}
void f1() {
  x=10;
  //f2();
  return;
}

int main() {
  int x; // fake
  x=1;
  f1();
  f2();
  x=5;
  return 1;
}
