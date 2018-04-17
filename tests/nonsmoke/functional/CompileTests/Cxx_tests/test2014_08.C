
class A
   {
     public:
          A(){}
   };

void
foobar()
   {
  // ROSE unparsers the initializer as "A::A();" but needs to be unparsed as "A();" to work with GNU g++ version 4.5.4 compiler.
     A objectA = A();
   }

