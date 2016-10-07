void foo()
{
  int a[100];
  int *b,c=0;
  void * p1;

  int &r =c;

}

class A {
  public:
    int a;

};

class B {
  int a;
  int b[100]; // not copy constructible, not assignable
};



