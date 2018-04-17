// cc.in68
// explicit dtor call w/o qualifier

class A {
public:
  //~A();
  void destroy(A &aref)
  {
    this->~A();
    aref.~A();
  }
};

int main()
{
  A *a1;
  a1->~A();

  A &a2;
  a2.~A();

  return 0;
}

