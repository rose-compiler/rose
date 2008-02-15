// cc.in67
// deref/overload problem

class Foo {
public:
  void f(int, int);
  int *f(int);     
};

int main()
{
  Foo foo;
  return *(foo.f(4));
}
