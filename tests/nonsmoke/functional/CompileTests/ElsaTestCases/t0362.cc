// t0362.cc
// another problem with implicit this

struct C {
  int foo(int);
  static int foo(int,int);
};

/*static*/ int C::foo(int x, int y)
{
  // attempt recursive call
  foo(x,y);
}

