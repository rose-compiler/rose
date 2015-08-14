void foo1();
int foo2(int,int*);

void foo1() {
  int a=100;
  int x;
  x=a;
  int z;
  foo2(x,&z);
}

int foo2(int x2, int* xp) {
  int y;
  int c=1;
  y=(x2-c)*12;
  int array[100];
  *xp=array[y];
  return 0;
}

int main() {
  foo1();
}
