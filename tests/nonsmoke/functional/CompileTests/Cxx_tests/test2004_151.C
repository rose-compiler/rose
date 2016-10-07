int xyz = 123;

/*

This is a comment that will loose a linefeed in its length!
*/

#if 1
namespace X
   {
  // int abc = 456;
     extern int abc;
   }

int X::abc = 789;

class A
   {
  // DQ (2/20/2010): Added "int" to as required for newer GNU compilers.
  // foo() const;
     int foo() const;
     int i;
   };
#endif
