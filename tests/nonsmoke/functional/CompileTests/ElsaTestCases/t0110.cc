// t0110.cc
// more with templates; testing the clone impl

typedef double A;

template <class T>
class Foo {
public:
  T x;
  A d;    // should be double
  
  Foo<T> *fooptr;
  Foo *fooptr2;
};

void f()
{
  typedef char *A;     // *not* seen by template definition

  Foo<int> y;
  Foo<int> z;

  y.x + z.x;

  y.d;                 // should be seen as having 'double' type
  y.fooptr;            // type: Foo<int>*
  y.fooptr2;           // type: Foo<int>*
}

