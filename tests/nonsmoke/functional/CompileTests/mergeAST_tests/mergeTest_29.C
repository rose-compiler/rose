// This demonstrates a case where the variable x is 
// both the namespace A's symbol table and the global symbol table.
// I think this is a bug. should only be in the namespace symbol table.
namespace A
   {
     extern int x;
   }
int A::x = 0;
