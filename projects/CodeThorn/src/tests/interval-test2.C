void foo1();
int foo2(int,int*);

void foo1() {
  int a=100;
  int x;
  x=a;
  int z;
  foo2(x,&z);
}

int foo2(int x, int* xp) {
  int y;
  int c=1;
  y=(x-c)*12;
  int a[100];
  *xp=a[y];
  return 0;
}

int main() {
  foo1();
}
