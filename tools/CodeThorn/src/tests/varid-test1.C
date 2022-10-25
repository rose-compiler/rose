class A {
public: 
 int x;
  int y;
};

class B : public A {
public:
  int x;
  int z;
};

int main() {
  int x;
  int* xp=&x;
  x=1;
  A a;
  A* ap;
  ap=&a;
  a.x=2+x;
  B b;
  B* bp=&b;
  b.x=3;
  ap->x=100;
  ap->x=101;
  ap->y=200;
  bp->y=200;
  *xp=300;
  return 0;
}
