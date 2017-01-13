/*
The following code compiles with g++, but not with the ROSE Identity translator.  
If you remove the "bad" class, then the "new foo *[20]" works great as declared 
in the goo() function (as opposed to breaking in class bad).

-Jeff

*/

class foo
   {
     public:
          foo() {}
         ~foo() {}
          int dummy ;
   };

#ifndef HIDE_THIS_FOR_GOOD_COMPILE
class bad
   {
     public:
          foo **m_foo ;
          bad() : m_foo(new foo *[20]) {}
         ~bad() {}
   };
#endif

void goo()
   {
     foo **data = new foo *[20] ;
   }

