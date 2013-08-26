// This test evaluates the ability of 1-level call context sensitivity to disambiguate the possible states
// inside a function based on the call sites. In this example we need context sensitivity to realize
// that threre are three instances of square(), each with a fixed value for its argument. As such, each
// instance has a fixed return value.
#pragma fuse lc(ccs, ccs, cp)
void CompDebugAssert(bool);

int square(int arg)
{
  return arg*arg;
}

int foo(int argFoo) {
  CompDebugAssert(square(1)==1);
  CompDebugAssert(square(2)==4);
  CompDebugAssert(square(3)==9);
}

void main(int argc, char** argv)
{
  int a=1;
  foo(123);
  foo(789);
}

