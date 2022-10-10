int foo1();
int foo2(int);

int foo1() {
  int a=100;
  int x;
  x=a;
  int y;
  y=foo2(x);
  y=y+1;
  return y;
}

int foo2(int x) {
  int y;
  int c=2;
  y=(x-c)*12;
  return y;
}

int foo3(int a) {
  int x;
  x=100;
  switch(a) {
  case 1: {
    x=1;
    break;
  }
 case 2:
   x=2;
 case 3:
   x=x+1;
   break;
 case 55:
   x=55;
 default:
   x=x+1;
  }
  return x;
}


int main() {
  int z1;
  int z2;
  z2=1;
  z1=foo1();
  z2++;
  z2=z2+z1;
  int z3;
  z3=1;
  z3=foo3(z3);
  return 0;
}
