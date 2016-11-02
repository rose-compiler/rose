// cc.in31
// explicit calls to some builtins

class Foo {
  void operator delete[]();
};

int main()
{
  void *x;
  
  operator delete (x);
  operator delete[] (x);
}
