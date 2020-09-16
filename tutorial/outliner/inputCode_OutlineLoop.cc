#include <stdio.h>
namespace N
{
  class A
  {
    int foo (void) const { return 7; }
    int bar (void) const { return foo () / 2; }
  public:
    int biz (void) const
    {
      int result = 0;
#pragma rose_outline
      for (int i = 1; i <= foo (); i++)
        for (int j = 1; j <= bar (); j++)
          result += i * j;
      return result;
    }
  };
}

extern "C" int printf (const char* fmt, ...);

int main ()
{
  N::A x;
  printf ("%d\n", x.biz ()); // Prints '168'
  return 0;
}
