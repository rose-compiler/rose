
class A {
public:
  int x;
  int y;
};

int g1=2;

int f(A* ap) {
  ap->x=5;
  ap->y=ap->x+1;
  g1++;
  return ap->x;
}

int main() {
  int x1=1;
  x1++;
  A* ap=new A();
  ap->x=x1;
  A& ar=*new A();
  ar.x=x1;
  f(ap);
  f(&ar);
  delete ap;

  return 0;
}
