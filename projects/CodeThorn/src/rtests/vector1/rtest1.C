#if 0
class A {
public:
  A() { x=0; }
  int x;
  ~A() { x=5; }
};
#endif

void func() {
  int* a=new int();
  int x;
  int y;
  int z;
  x=1;
  z=2;
  if(x==1)
    x=1;
  if(x==1) {
    x=2;
  } else {
    x=3;
    y=4;
  }
  *a=5;
  y=x;
  z=1;
  if(x>1) {
    z=z+x+y;
  }
  (*a)=z;
  delete a;
}

int main() {
  func();
  return 0;
}
