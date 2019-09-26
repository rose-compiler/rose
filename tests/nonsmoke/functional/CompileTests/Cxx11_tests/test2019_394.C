
struct A
   {
     typedef int mytype;
   };

struct X
   {
     struct Y
        {
        };
   };

void foobar()
   {
     A::mytype X::Y::*p;
   }
 
