// t0108.cc
// simple template argument tests

// this is a version of t0108.cc that doesn't attempt to change the
// template parameter names between the declaration and definition of
// the member function

template <int n>
class Foo {
  public:
  int x;
  Foo() { x=n; }    // assign from template argument
  int f();
  int g();
};

template <int n>
int Foo<n>::f()
{
  return n;
}

// NOTE: this name-change feature is not tested here; see t0108.cc
// should be legal to change parameter name, I think
//  template <int m>
//  int Foo<m>::g()
//  {
//    return m;
//  }
template <int n>
int Foo<n>::g()
{
  return n;
}

int main()
{
  Foo<3> f;
  f.f();
  f.x;
  
  return f.g();
}
