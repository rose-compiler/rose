// t0343.cc
// 5.3.1 para 9: ambiguity between ~X() as destructor call
// and ~X() as bitwise complement after conversion to int

extern "C" int printf(char const *format, ...);

struct X {
  X()                 { printf("constructor invocation\n"); }
  ~X()                { printf("destructor invocation\n"); }
  operator int ()     { printf("conversion to int\n"); }

  void foo();
};

void X::foo()
{
  // definitely a conversion to int
  printf("test 1\n");
  0 + X();

  // definitely a destructor invocation
  printf("test 2\n");
  this->X::~X();

  // also a destructor invocation, I think, but gcc does not like it ...
  //printf("test 3\n");
  //X::~X();

  // nominally ambiguous, but cppstd says prefer conversion + bitwise
  // complement (can see that Elsa agrees with "-tr prettyPrint".. I
  // can't think of a way to make that automatic here...)
  printf("test 4\n");
  ~X();

  printf("end of foo()\n");
}

int main()
{
  X x;
  x.foo();
}
