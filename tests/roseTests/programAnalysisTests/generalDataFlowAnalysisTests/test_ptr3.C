int *p;
int x,a,b;

void foo()
{
  x=10;
  a=10;
  int c = b;
  p = &x;
  #pragma rose p: Aliases:{ x }{}\
    x: Aliases:{ }{}\
    a: Aliases:{ }{}\
    b: Aliases:{ }{}
}

void main()
{
  x = 79;
  a=20;
  int *r;

  r = &a;
  #pragma rose p: Aliases:{ }{}\
    x: Aliases:{ }{}\
    a: Aliases:{ }{}\
    b: Aliases:{ }{}\
    r: Aliases:{ a }{}

  b = 8;

  foo();
  #pragma rose p: Aliases:{ x }{}\
    x: Aliases:{ }{}\
    a: Aliases:{ }{}\
    b: Aliases:{ }{}\
    r: Aliases:{ a }{}

  x  = b + a + x;

  p = &b;
  #pragma rose p: Aliases:{ b }{}\
    x: Aliases:{ }{}\
    a: Aliases:{ }{}\
    b: Aliases:{ }{}\
    r: Aliases:{ a }{}
}

