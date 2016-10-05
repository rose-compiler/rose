// g0005.cc
// __builtin_expect

void glorf(int);

void foo(int x)
{
  if (__builtin_expect(x == 5, 0)) {
    glorf(1);
  }
  else {
    glorf(2);
  }
}
