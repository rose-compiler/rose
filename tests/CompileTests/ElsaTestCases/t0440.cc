// t0440.cc
// use a default argument in a DQT
// more complex version of t0439.cc

template <class T = int>
struct A {};

template <>
struct A</*int*/> {      // !!
  int x;
  
  A *f1();
  A<> *f2();
  A<int> *f3();
};

A<> *A<int>::f1()
{
  return this;
}

A<int> *A<>::f2()
{
  return this;
}

A<int> *A<int>::f3() 
{
  return this;
}


int f()
{
  A<> a;
  a.x = 5;
  return a.x;
}
