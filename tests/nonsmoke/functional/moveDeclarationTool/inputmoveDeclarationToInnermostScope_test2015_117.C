// This is a copy of test2015_86.C
// This demonstrates a failing test 
// for a large code.

struct A
   {
     int x;
   };

void foobar( A* aaa )
   {
      int i1;

      if ( i1 == 7 ) {

         if ( i1 <= aaa->x ) {
         }
         else if ( i1 >= aaa->x ) {
         }
      }
   }
