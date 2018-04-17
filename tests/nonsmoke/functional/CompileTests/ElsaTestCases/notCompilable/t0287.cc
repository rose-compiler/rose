// t0287.cc
// default non-type template argument

template <class T, bool f = false>
class A {
  T t;
  bool getF() {
    return f;
  }
};

bool func()
{
  A<int> a;
  return a.getF();
}

bool func2()
{
  A<int, true> a;
  return a.getF();
}

bool func3()
{
  A<int, false> a;
  return a.getF();
}
