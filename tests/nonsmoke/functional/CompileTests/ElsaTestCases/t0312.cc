// t0312.cc
// ambiguous use of built-in assignment operator

struct A {
  operator int();
  operator unsigned int();
  operator double();
};


int main()
{
  A a;
  int i = 4;
  double d = 5;
  unsigned int u = 6;

  // for some reason, gcc and icc both accept these
  i = a;
  d = i;
  u = i;

  // even though they correctly reject this
  //ERROR(1): i += a;

  return 0;
}
