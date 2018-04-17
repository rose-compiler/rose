//  ./abiword-1.0.4-2/fl_SectionLayout-aKmk.i.cpp_out:/home/ballAruns/tmpfiles/./abiword-1.0.4-2/fl_SectionLayout-aKmk.i:16896:28: error: there is no variable called `pPair'

// gcc allows a struct to be referred to as a class and vice versa

struct A {
  public:
  int x;
};
class A a;

class B {
  public:
  int y;
};
struct B b;

int main() {
  a.x;
  b.y;
}
