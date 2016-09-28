// This is test code demonstrates the use of covariant return types.
// The test code works, but nothing is ROSE is currently identifying 
// the function with the covariant return type as being special (i.e. 
// having a covariant return type).

struct Base 
   {
     virtual Base* foobar();
   };

struct Derived : public Base 
   {
     Derived* foobar();
   };


