#define SIMPLE 1
namespace N
{
  class A
  {
 // Note that these are private, so the outline function will not 
 // be able to access them. The generated friend function must
 // also be output with global qualification to permit it reference
 // the global scope version of the function instead of generating 
 // a reference to a function that has not been defined with a scope.

    int foo (void) const { return 7; }
#if !SIMPLE
    int bar (void) const { return foo () / 2; }
#endif

  public:
    int biz (void) const
    {
   // Build a reference to test the AST copy mechanism.
      foo();

      int result = 0;
      typedef A* pointerToA;
#pragma rose_outline
      for (int i = 1; i <= foo (); i++)
#if !SIMPLE
        for (int j = 1; j <= bar (); j++)
          result += i * j;
#else
      result += i;
#endif
      return result;
    }
  };
}

#if !SIMPLE
extern "C" int printf (const char* fmt, ...);
#endif

#if !SIMPLE
int main ()
{
  N::A x;
#if !SIMPLE
  printf ("%d\n", x.biz ()); // Prints '168'
#endif
  return 0;
}
#endif
