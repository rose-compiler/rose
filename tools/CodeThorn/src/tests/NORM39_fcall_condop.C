void f() {}

int g() { return 1; }

int main() {
  int x;
  g();
  f();
  x?f():f();
}
   
