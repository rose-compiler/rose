// t0477.cc
// template class with nested class and instance declared together

template < class T >
class A {
private:
  struct B { } b;
};

void foo()
{
  A<int> x;
}
