// New version I got on March 21/2023
struct A
{
  static const int MAX = 42; 
};

int foobar()
{
  int abvar; 
#pragma rose_outline
  abvar = A::MAX; 
  return 0; 
}


