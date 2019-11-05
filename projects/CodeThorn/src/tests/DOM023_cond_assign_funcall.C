int f() {
  int x;
  x=1;
  return x;
}

int f();

int main() {
  int y;
  y=1;
  if((y=f())==0) {
    y=y+2;
  }
}
