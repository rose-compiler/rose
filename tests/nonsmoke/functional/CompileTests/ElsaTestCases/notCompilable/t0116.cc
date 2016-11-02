// t0116.c
// experimenting with overloading; be careful with line numbers!

int foo(int i)    // line 4
{
  return i;
}

float foo(float f)  // line 9
{
  return f;
}

void notOverloaded() {}     // line 14

void bar()
{
  __testOverload(notOverloaded(), 14);    // turn on overload resolution

  __testOverload(foo(3), 4);
  __testOverload(foo(3.4f), 9);
}

