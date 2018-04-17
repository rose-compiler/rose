// cc.in13
// inline member functions

class Foo {
public:
  int func()
  {
    //ERROR(1): return y;
    return x;
  }

  //ERROR(3): int func();
  
  //ERROR(4): int func() { return 5; }

  int bar();
  //ERROR(2): int bar();

  int x;
};

int main()
{
  Foo x;
  x.func();
}
