// Test for hiding class name where global qualification is required.

struct A
   {
     struct B
        {
        };
   };


namespace C
   {
     typedef int A;

  // Name qualification is required here (though GNU 4.2 does not appear to require it, EDG does).
     ::A::B foo();
   }
