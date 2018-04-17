#if 0
namespace N {
 class A {
  int foo (void) const { return 7; }
  int bar (void) const { return foo () / 2; }
    public:
#endif

    int biz (void)
    {
      int result = 0;
// #pragma rose_outline
#if 0
      for (int i = 1; i <= foo (); i++)
#else
      for (int i = 0; int k = i; k++)
#endif
      {
#if 0
        for (int j = 1; j <= biz(); j++)
#else
//      int j;
#endif
//      { 
     // result += i * j; 
     // i = 0;
//      }

//    return result;
      return 0;
      }
    }

#if 0
 };
}
#endif

#if 0
extern "C" int printf (const char* fmt, ...);

int main ()
{
  N::A x;
  printf ("%d\n", x.biz ()); // Prints '168'
  return 0;
}
#endif
