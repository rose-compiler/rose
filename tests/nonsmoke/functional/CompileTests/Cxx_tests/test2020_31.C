class A
   {
     public:
          int foo();
   };


class B : public A
   {
     public:
       // This is an old style using-declaration.
       // BUG: This is unparsed without the public access level.
          A::foo;
   };

void myFunction()
   {
     B o;
     o.foo();
   }
