// t0354.cc
// type of string literal affects semantics

extern "C" {
  int printf(char const *fmt, ...);
}

void foo(char*)                      // line 8
{
  printf("foo(char*)\n");
}

void foo(char const*)                // line 13
{
  printf("foo(char const*)\n");
}

int main()
{
  __testOverload(foo("hi"), 13);
  return 0;
}
