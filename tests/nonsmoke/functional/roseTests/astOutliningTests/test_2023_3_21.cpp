/*
Example code demonstrating outliner error in handling static const data members

Compiler will do constant propagation for static const data members of integer or enum types

However, outiling will introduce & operation before the members, which prevents compilers from doing constant propagation. 
This will cause linking error. 

To address this problem,  the outliner should do the constant propagation before outlining the code.

*/

enum Color {
  Bronze,
  Silver,
  Gold
};

struct A
{
   static const int MAX = 42; 
   static const Color c = Gold; 
};

int main()
{
   int abvar; 
#pragma rose_outline
    abvar = A::MAX + (int)(A::c); 
  return 0; 
}
