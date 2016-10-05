/*
Below is a reproducer for the pathscale destructor compiler bug we were
seeing.  According to C++ standard section 12.4 paragraph 6 and section
6.6.3 paragraph 2, having a return statement in a destructor should not
cause early exit from the destruction process.  For the pathscale
compiler, a return will prematurely terminate the destruction process.
-Jeff (Keasler)
*/

#include <stdio.h>

class A {
public:
   A() { printf("construct A\n") ; } ;
   ~A() {printf("destruct A\n") ; } ;
} ;

class B : public A {
public:
   B() {} ;
   virtual ~B()
      {
     // Notice that the return is dropped in ROSE (maybe in EDG, since it is a normalization consistant with the standard).
        return ; 
      } ;
} ;

int main()
{
   B *testCase = new B ;
   delete testCase ;

   return 0 ;
} 
