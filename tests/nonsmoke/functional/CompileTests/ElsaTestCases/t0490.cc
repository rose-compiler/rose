// t0490.cc
// conversions of function pointers
// normalization of parameter types is the thing?

typedef int (*param)(char *buf);
void foo(param);

typedef int (*arg)(char * const inbuf);
void bar(arg x)
{
  foo((arg)x);
}

typedef int (*arg2)(char * __restrict inbuf);
void bar2(arg2 x)
{
  // I actually think it is unsound to allow this, but the rules
  // for C++ function types say to drop cv, and the rules for 
  // C99 restrict say to treat it like cv, so here we are.
  foo((arg2)x);
}



// but don't drop 'const' for the parameter inside
// the function!

int g(int &);

int f(int const x)
{
  int const y = 4;

  //ERROR(1): g(y);      // obviously wrong

  //ERROR(2): g(x);      // wrong only if we remember 'x' is const
}
