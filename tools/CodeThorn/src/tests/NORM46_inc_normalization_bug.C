class A {
  int x;
public:
  int m() {
    this->x=this->x+1;
    return this->x;
  }
};

struct X {
  int x;
};

int main() {
  A a;
  a.m();
  int x;
  x=x+x++;
  X b;
  b.x++;
  struct X* p;
  p->x=p->x+1;
}
