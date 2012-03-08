int init1() {
  return 1;
}

int inc1(int x) {
  return x+1;
}

int pinc(int* pint) {
  return (*pint)++;
}

int main() {
  int x=init1();
  int y=inc1(x);
  int z=pinc(&y);
  int a=x+y+z+inc1(x)+inc1(inc1(x));
  return 0;
}
