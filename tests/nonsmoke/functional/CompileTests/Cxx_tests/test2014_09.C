
namespace X
   {
     class ABC
        {
          public:
               ABC(){}
        };
   }

void
foobar()
   {
  // ROSE unparsers the initializer as "A::A();" but needs to be unparsed as "A();" to work with GNU g++ version 4.5.4 compiler.
      X::ABC objectA = X::ABC();
   }

