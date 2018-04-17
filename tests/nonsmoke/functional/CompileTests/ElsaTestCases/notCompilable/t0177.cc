// t0177.cc
// a weird "typename" thing

class A {
  typedef int foo;
};

void bar()
{
  typename A::foo();
}
