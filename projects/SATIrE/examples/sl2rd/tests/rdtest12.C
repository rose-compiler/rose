int globalVar;

void f() {
  globalVar = globalVar +1;
}

int main() {
  int x,y,z;
  x=5;
  y=10;
  z=x+y;
  globalVar=z;
  f();
  globalVar=globalVar+z;
  return 0;
}
