// t0111.cc
// some nontype template arguments
          
template <int n>
class Foo {
public:
  int arr[n];
};

void f()
{
  Foo<3> x;
  Foo<1+2> y;   // same type!
  Foo<2+2> z;   // different type!
  x.arr;
  y.arr;
  z.arr;
  
  //ERROR(1): Foo<-5> w;    // negative array size not allowed
}


template <class T>
class A {                    
  // will be error if it thinks it knows the size of T
  int arr[sizeof(T) - 10];
};

void g()
{
  typedef int blah[30];
  A<blah> a;
}
