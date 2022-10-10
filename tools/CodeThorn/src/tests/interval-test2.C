void foo1();
int foo2(int,int*);
int glob;

void foo1() {
  int a=100;
  int x;
  x=a;
  int z;
  z=400;
  foo2(x,&z);
}

int foo2(int x2, int* xp) {
  int y;
  int c=1;
  y=(x2-c)*12;
  int array[100];
  *xp=array[y];
  glob=y;
  return 0;
}

int main() {
  glob=1;
  foo1();
  glob++;
  return 0;
}
