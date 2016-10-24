// 8.2.cc

struct S {
  S(int);
};

void foo(double a)
{
  S w(int(a));   // function declaration
  S x(int());    // function declaration
  S y((int)a);   // object declaration
  S z = int(a);  // object declaration
}
