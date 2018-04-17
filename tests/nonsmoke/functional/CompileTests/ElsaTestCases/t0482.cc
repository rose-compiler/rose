// t0482.cc
// ambiguous NewExpression

void *operator new (unsigned size, int x);

void *foo()
{
  int x = 0;
  typedef int y;
  return new (x)(y);     // placement x, then parenthesized type-id y
}

void *bar()
{
  typedef int x;
  int y = 0;
  return new (x)(y);     // parenthesized type-id x, new-initializer
}
