// a header file with an inlined function
inline int foo (int a, int b)
{
  int c;
#pragma rose_outline
   c=a+b;

   return c;
}

